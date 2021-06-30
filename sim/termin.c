#include "termin.h"
#include <stdio.h>
#include <stdlib.h>

void termin_handle_typed(struct termin *termin, struct event *event) {
  uchar code = event->special.typed.unicode;
  fprintf(stdout, "typed character %u ascci %c\n", event->special.typed.unicode,
          code);
  if (code == BACKSPACE || code == DELETE) {
    fprintf(stdout, "must delete\n");
  } else if (code == ENTER) {
    fprintf(stdout, "must send message\n");
  } else {
    fprintf(stdout, "must type: %c\n", code);
  }
}

u8 termin_read_event(struct termin *termin) {
  struct event *event = window_next_event(termin->window);
  if (event == NULL)
    return 0;
  switch (event->type) {
  case EVENT_TYPED:
    termin_handle_typed(termin, event);
    break;
  }
  return 1;
}

void termin_draw(struct termin *termin) {
  // fprintf(stdout, "termin_draw\n");
  window_clear(termin->window, BLACK);
  window_begin_drawing(termin->window);
  window_clear(termin->window, WHITE);
  window_draw_rectangle_from_recpoints(termin->window, BLACK, 2.0,
                                       &termin->output_view);
  window_draw_rectangle_from_recpoints(termin->window, BLACK, 2.0,
                                       &termin->input_view);
  window_end_drawing(termin->window);
}

void termin_handle_events(struct termin *termin) {
  window_read_events(termin->window);
  u8 must_read = 1;
  while (must_read) {
    must_read = termin_read_event(termin);
  }
}

void termin_run(struct termin *termin) {
  window_run(termin->window);
  while (termin->window->must_run) {
    termin_handle_events(termin);
    termin_draw(termin);
  }
}

struct termin *termin_create(const char *title) {
  struct termin *termin = malloc(sizeof(struct termin));
  system_initialize();
  termin->view_size[0] = 256.0;
  termin->view_size[1] = 512.0;
  termin->window =
      window_create(title, termin->view_size[0], termin->view_size[1]);
  rectangle_points_set(&termin->output_view, 0.0, 0.0, termin->view_size[0],
                       termin->view_size[1] / 2.0);
  rectangle_points_set(&termin->input_view, 0.0, termin->view_size[1] / 2.0,
                       termin->view_size[0], termin->view_size[1]);
  return termin;
}

void termin_free(struct termin *termin) {
  window_free(termin->window);
  free(termin);
}
