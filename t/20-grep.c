#include "../ext/tap.c/tap.c"

#include "mIterator.c"

const int limit = 4;
int free_called = 0;
int grep_ctx_freed = 0;

void freefn(void *v) {
  (void)v;
  ++free_called;
}

void grepfreefn(void *v) {
  (void)v;
  ++grep_ctx_freed;
}

mit_status_t nextfn(void *ctx, void **result) {
  int *c = ctx;
  if (*c >= limit) { return MIT_EXHAUSTED; }
  else { ++(*c); *result = c; return MIT_OK; }
}

mit_status_t grepfn(void *value, void *ctx, int *matches) {
  (void)ctx;
  *matches = !(*((int *)value) % 2);
  return MIT_OK;
}

int main(void) {
  int ctx = 0;
  mit_result_t *res;
  mit_t *mit = mit_new(nextfn, &ctx, freefn);
  mit_t *even = mit_grep(mit, grepfn, NULL, grepfreefn);

  tap_plan(14);

  /* item 1 */
  tap_ok((res = mit_next(even)) != NULL, "next(1) returns a result");
  tap_is_int(res->status, MIT_OK, "next(1) returns OK");
  tap_is_int(*((int *) res->value), 2, "next(1) returns correct value");
  tap_is_int(mit_status(even), MIT_OK, "iterator status is OK");

  /* item 2 */
  tap_ok((res = mit_next(even)) != NULL, "next(2) returns a result");
  tap_is_int(res->status, MIT_OK, "next(2) returns OK");
  tap_is_int(*((int *) res->value), 4, "next(2) returns correct value");
  tap_is_int(mit_status(even), MIT_OK, "iterator status is OK");

  /* exhausted */
  tap_ok((res = mit_next(even)) != NULL, "next(3) returns a result");
  tap_is_int(res->status, MIT_EXHAUSTED, "next(3) returns EXHAUSTED");
  tap_ok(res->value == NULL, "next(3) returns NULL ");
  tap_is_int(mit_status(even), MIT_EXHAUSTED, "iterator status is EXHAUSTED");

  mit_free(even);

  tap_is_int(free_called, 1, "inner iterator freed");
  tap_is_int(grep_ctx_freed, 1, "grep context freed");

  return tap_finish();
}
