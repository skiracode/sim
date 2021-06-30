#include "termin.h"

void main(void) {
  struct termin *termin = termin_create("sim");
  termin_run(termin);
  termin_free(termin);
}
