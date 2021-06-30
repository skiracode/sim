#pragma once
#include "types.h"

#define EVENT_BUFFER_SIZE 256

struct window;
struct monitor;
struct window_internal;
struct event;
struct rectangle;
struct typed_event;
union special;
struct rectangle_points;

struct rectangle {
  f64 size[2];
  f64 position[2];
};

struct rectangle_points {
  f64 top_left[2];
  f64 bottom_right[2];
};

struct typed_event {
  u32 unicode;
  u8 repeat;
};

union special {
  struct typed_event typed;
};

struct event {
  u32 type;
  union special special;
};

enum {
  EVENT_NONE = 0,
  EVENT_TYPED = 1,
};

enum {
  BACKSPACE = 8,
  DELETE = 127,
  ENTER = 13,
};

enum {
  BLACK = 0,
  WHITE = 1,
};

struct monitor {
  u32 identifier;
  u32 size[2];
};

struct window {
  u32 must_run;
  struct monitor *monitor;
  struct rectangle view;
  f64 proportion;
  f64 inverse;
  const char *title;
  u32 size[2];
  u32 pixel_buffer_size[2];
  struct event event_buffer[EVENT_BUFFER_SIZE];
  u32 event_index;
  u32 event_count;
  struct window_internal *internal;
};

struct window *window_create(const char *title, u32 pixel_buffer_width,
                             u32 pixel_buffer_height);
void system_initialize();
struct monitor *monitor_get_first();
void system_set_current_monitor(struct monitor *monitor);
void monitor_free(struct monitor *monitor);
void window_free(struct window *window);
void window_run(struct window *window);
void window_read_events(struct window *window);
struct event *window_next_event(struct window *window);
void window_begin_drawing(struct window *window);
void window_end_drawing(struct window *window);
void window_clear(struct window *window, u32 color);
void rectangle_points_set(struct rectangle_points *recpoints, f64 y1, f64 x1,
                          f64 y2, f64 x2);
void window_draw_rectangle(struct window *window, u32 color, f64 thickness,
                           f64 y1, f64 x1, f64 y2, f64 x2);
void window_draw_rectangle_from_recpoints(struct window *window, u32 color,
                                          f64 thickness,
                                          struct rectangle_points *recpoints);