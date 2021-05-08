#include "../ext/tap.c/tap.c"

#include "mIterator.c"

const int limit = 10;

mit_status_t nextfn(void *ctx, void **result) {
  int *c = ctx;
  if (*c >= limit) { return MIT_EXHAUSTED; }
  else { ++(*c); *result = c; return MIT_OK; }
}

int main(void) {
  int ctx = 0;
  mit_result_t *res;
  mit_t *mit = mit_new(nextfn, &ctx, NULL);

  tap_plan(18);

  tap_ok((res = mit_next(mit)) != NULL, "next(1) returns a result");
  tap_is_int(*((int *)res->value), 1, "next(1) value is correct");
  tap_is_int(res->status, MIT_OK, "next(1) status is OK");

  tap_is_int(mit_skip(mit, 0), MIT_OK, "skip(0) returns OK");

  tap_ok((res = mit_next(mit)) != NULL, "next(2) returns a result");
  tap_is_int(*((int *)res->value), 2, "next(2) value is correct");
  tap_is_int(res->status, MIT_OK, "next(2) status is OK");

  tap_is_int(mit_skip(mit, 2), MIT_OK, "skip(2) returns OK");

  tap_ok((res = mit_next(mit)) != NULL, "next(3) returns a result");
  tap_is_int(*((int *)res->value), 5, "next(3) value is correct");
  tap_is_int(res->status, MIT_OK, "next(3) status is OK");

  tap_ok((res = mit_nth(mit, 0)) != NULL, "nth(0) returns a result");
  tap_is_int(*((int *)res->value), 6, "nth(0) value is correct");
  tap_is_int(res->status, MIT_OK, "nth(0) status is OK");

  tap_ok((res = mit_nth(mit, 3)) != NULL, "nth(0) returns a result");
  tap_is_int(*((int *)res->value), 10, "nth(3) value is correct");
  tap_is_int(res->status, MIT_OK, "nth(3) status is OK");

  tap_is_int(mit_next(mit)->status, MIT_EXHAUSTED, "iterator is exhausted");

  mit_free(mit);

  return tap_finish();
}
