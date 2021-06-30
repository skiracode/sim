#pragma once
#include "system.h"

#define CHARS_PER_ENTRY 128
#define ENTRIES_PER_TERMIN 16

struct termin;
struct entry;

struct entry {
  u32 chars_count;
  u8 characters[CHARS_PER_ENTRY + 1];
};

struct termin {
  struct rectangle_points output_view;
  struct rectangle_points input_view;
  f64 view_size[2];
  struct window *window;
  u32 current_entry;
  u32 first_entry;
  struct entry entries[ENTRIES_PER_TERMIN];
};

struct termin *termin_create(const char *title);
void termin_free(struct termin *termin);
void termin_run(struct termin *termin);
