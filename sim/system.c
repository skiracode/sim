#include "system.h"
#include <allegro5/allegro5.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_font.h>
#include <stdio.h>
#include <stdlib.h>
struct window_internal {
  ALLEGRO_EVENT_QUEUE *queue;
  ALLEGRO_DISPLAY *display;
  ALLEGRO_COLOR white;
  ALLEGRO_COLOR black;
  ALLEGRO_EVENT event;
  ALLEGRO_BITMAP *bitmap;
  ALLEGRO_FONT *font;
};
void system_set_current_monitor(struct monitor *monitor) {
  al_set_new_display_adapter(monitor->identifier);
}
void system_initialize() {
  al_init();
  al_init_primitives_addon();
  al_init_font_addon();
  al_install_keyboard();
  al_install_mouse();
  al_set_new_bitmap_flags(ALLEGRO_NO_PRESERVE_TEXTURE);
}
struct monitor *monitor_get_first() {
  struct monitor *monitor = malloc(sizeof(struct monitor));
  monitor->identifier = 0;
  ALLEGRO_MONITOR_INFO info;
  al_get_monitor_info(monitor->identifier, &info);
  monitor->size[0] = info.y2 - info.y1;
  monitor->size[1] = info.x2 - info.x1;
  return monitor;
}

void monitor_free(struct monitor *monitor) { free(monitor); }

void window_adjust_view(struct window *window, f64 height, f64 width) {
  f64 difference, half;
  if (height > window->inverse * width) {
    window->view.size[1] = width;
    window->view.size[0] = width * window->inverse;
    difference = height - window->view.size[0];
    half = difference / 2.0;
    window->view.position[0] = half;
    window->view.position[1] = 0;
  } else {
    window->view.size[0] = height;
    window->view.size[1] = height * window->proportion;
    difference = width - window->view.size[1];
    half = difference / 2.0;
    window->view.position[0] = 0;
    window->view.position[1] = half;
  }
}

struct window *window_create(const char *title, u32 pixel_buffer_height,
                             u32 pixel_buffer_width) {
  struct window *window = malloc(sizeof(struct window));
  window->monitor = monitor_get_first();
  window->title = title;
  fprintf(stdout, "monitor->size = %u, %u\n", window->monitor->size[0],
          window->monitor->size[1]);
  f64 size_y = window->monitor->size[0];
  size_y *= 3.0 / 4.0;
  f64 size_x = window->monitor->size[1];
  size_x *= 3.0 / 4.0;
  window->size[0] = size_y;
  window->size[1] = size_x;
  int y = (window->monitor->size[0] - window->size[0]) / 2;
  int x = (window->monitor->size[1] - window->size[1]) / 2;
  al_set_new_window_position(x, y);
  fprintf(stdout, "window->size = %u, %u\n", window->size[0], window->size[1]);
  window->pixel_buffer_size[0] = pixel_buffer_height;
  window->pixel_buffer_size[1] = pixel_buffer_width;
  window->proportion = (f64)pixel_buffer_width / (f64)pixel_buffer_height;
  window->inverse = 1.0 / window->proportion;
  window_adjust_view(window, size_y, size_x);
  window->internal = malloc(sizeof(struct window_internal));
  window->internal->black = al_map_rgba(0, 0, 0, 255);
  window->internal->white = al_map_rgba(255, 255, 255, 255);
  window->internal->queue = al_create_event_queue();
  al_set_new_display_flags(ALLEGRO_WINDOWED | ALLEGRO_RESIZABLE);
  return window;
}
void window_internal_free(struct window_internal *internal) {
  al_destroy_bitmap(internal->bitmap);
  al_destroy_display(internal->display);
  al_destroy_event_queue(internal->queue);
  free(internal);
}
void window_free(struct window *window) {
  window_internal_free(window->internal);
  free(window);
}
u8 window_add_typed_event(struct window *window) {
  struct event *event = &window->event_buffer[window->event_count];
  event->type = EVENT_TYPED;
  struct typed_event *typed = &event->special.typed;
  typed->unicode = window->internal->event.keyboard.unichar;
  if (typed->unicode == 0)
    return 0;
  if (window->internal->event.keyboard.repeat == true)
    typed->repeat = 1;
  else
    typed->repeat = 0;
  return 1;
}

void window_process_event(struct window *window, ALLEGRO_EVENT *event) {
  u8 must_increment = 0;
  switch (event->type) {
  case ALLEGRO_EVENT_DISPLAY_CLOSE:
    window->must_run = 0;
    break;
  case ALLEGRO_EVENT_DISPLAY_RESIZE:
    window_adjust_view(window, event->display.height, event->display.width);
    al_acknowledge_resize(event->display.source);
    break;
  case ALLEGRO_EVENT_KEY_CHAR:
    if (window->event_count < EVENT_BUFFER_SIZE)
      must_increment = window_add_typed_event(window);
    break;
  }
  if (must_increment)
    window->event_count++;
}
void window_read_events(struct window *window) {
  window->event_count = 0;
  window->event_index = 0;
  while (al_get_next_event(window->internal->queue, &window->internal->event) ==
         true) {
    window_process_event(window, &window->internal->event);
  }
  // update and draw
}
void window_run(struct window *window) {
  system_set_current_monitor(window->monitor);
  window->internal->display =
      al_create_display(window->size[1], window->size[0]);
  al_clear_to_color(window->internal->black);
  al_flip_display();
  window->internal->bitmap = al_create_bitmap(window->pixel_buffer_size[1],
                                              window->pixel_buffer_size[0]);
  al_register_event_source(window->internal->queue,
                           al_get_keyboard_event_source());
  al_register_event_source(window->internal->queue,
                           al_get_mouse_event_source());
  al_register_event_source(
      window->internal->queue,
      al_get_display_event_source(window->internal->display));
  window->must_run = 1;
  window->event_count = 0;
  window->event_index = 0;
}
struct event *window_next_event(struct window *window) {
  if (window->event_index == window->event_count)
    return NULL;
  struct event *event = &window->event_buffer[window->event_index];
  window->event_index++;
  return event;
}

void window_end_drawing(struct window *window) {
  al_set_target_backbuffer(window->internal->display);
  float sx = 0.0f;
  float sy = 0.0f;
  float sw = window->pixel_buffer_size[1];
  float sh = window->pixel_buffer_size[0];
  float dx = window->view.position[1];
  float dy = window->view.position[0];
  float dw = window->view.size[1];
  float dh = window->view.size[0];
  al_draw_scaled_bitmap(window->internal->bitmap, sx, sy, sw, sh, dx, dy, dw,
                        dh, 0);
  al_flip_display();
}

void window_begin_drawing(struct window *window) {
  al_set_target_bitmap(window->internal->bitmap);
}

ALLEGRO_COLOR window_map_color(struct window *window, u32 color) {
  switch (color) {
  case WHITE:
    return window->internal->white;
  }
  return window->internal->black;
}

void window_draw_rectangle_from_recpoints(struct window *window, u32 color,
                                          f64 thickness,
                                          struct rectangle_points *recpoints) {
  window_draw_rectangle(window, color, thickness, recpoints->top_left[0],
                        recpoints->top_left[1], recpoints->bottom_right[0],
                        recpoints->bottom_right[1]);
}

void window_draw_rectangle(struct window *window, u32 color, f64 thickness,
                           f64 y1, f64 x1, f64 y2, f64 x2) {
  al_draw_rectangle(x1, y1, x2, y2, window_map_color(window, color), thickness);
}

void window_clear(struct window *window, u32 color) {
  al_clear_to_color(window_map_color(window, color));
}

void rectangle_points_set(struct rectangle_points *recpoints, f64 y1, f64 x1,
                          f64 y2, f64 x2) {
  recpoints->top_left[0] = y1;
  recpoints->top_left[1] = x1;
  recpoints->bottom_right[0] = y2;
  recpoints->bottom_right[1] = x2;
}
