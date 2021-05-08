#include "../ext/tap.c/tap.c"

#include "mIterator.c"

int free_called = 0;

void freefn(void *v) {
  (void)v;
  ++free_called;
}

int main(void) {
  char ctx[] = "ctx";
  mit_t *mit = mit_new(NULL, ctx, freefn);

  tap_plan(8);

  tap_ok(mit != NULL, "mit_new returns a valid pointer");

  tap_is_int(mit_status(mit), MIT_OK, "initial status OK");
  tap_is_int(mit_is_ready(mit), 1, "iterator starts ready");
  tap_is_int(mit_is_error(mit), 0, "iterator starts no error");
  tap_is_int(mit_is_exhausted(mit), 0, "iterator starts not exhausted");
  tap_is_int(mit_is_finite(mit), 0, "iterator defaults to infinite");

  tap_ok(mit_ctx(mit) == ctx, "mit_ctx returns correct pointer");

  mit_free(mit);

  tap_is_int(free_called, 1, "freefn called");

  return tap_finish();
}
