#include "../ext/tap.c/tap.c"

#include "mIterator.c"

const int limit = 2;
int free_called = 0;
int map_ctx_freed = 0;

void freefn(void *v) {
  (void)v;
  ++free_called;
}

void mapfreefn(void *v) {
  (void)v;
  ++map_ctx_freed;
}

mit_status_t nextfn(void *ctx, void **result) {
  int *c = ctx;
  if (*c >= limit) { return MIT_EXHAUSTED; }
  else { ++(*c); *result = c; return MIT_OK; }
}

mit_status_t mapfn(void *value, void *ctx, void **result) {
  int *raw = value, *doubled = ctx;
  *doubled = (*raw) * 2;
  *result = doubled;
  return MIT_OK;
}

int main(void) {
  int ctx = 0, mctx;
  mit_result_t *res;
  mit_t *mit = mit_new(nextfn, &ctx, freefn);
  mit_t *doubled = mit_map(mit, mapfn, &mctx, mapfreefn);

  tap_plan(14);

  /* item 1 */
  tap_ok((res = mit_next(doubled)) != NULL, "next(1) returns a result");
  tap_is_int(res->status, MIT_OK, "next(1) returns OK");
  tap_is_int(*((int *) res->value), 2, "next(1) returns correct value");
  tap_is_int(mit_status(doubled), MIT_OK, "iterator status is OK");

  /* item 2 */
  tap_ok((res = mit_next(doubled)) != NULL, "next(2) returns a result");
  tap_is_int(res->status, MIT_OK, "next(2) returns OK");
  tap_is_int(*((int *) res->value), 4, "next(2) returns correct value");
  tap_is_int(mit_status(doubled), MIT_OK, "iterator status is OK");

  /* exhausted */
  tap_ok((res = mit_next(doubled)) != NULL, "next(3) returns a result");
  tap_is_int(res->status, MIT_EXHAUSTED, "next(3) returns EXHAUSTED");
  tap_ok(res->value == NULL, "next(3) returns NULL ");
  tap_is_int(mit_status(doubled), MIT_EXHAUSTED, "iterator status is EXHAUSTED");

  mit_free(doubled);

  tap_is_int(free_called, 1, "inner iterator freed");
  tap_is_int(map_ctx_freed, 1, "map context freed");

  return tap_finish();
}
