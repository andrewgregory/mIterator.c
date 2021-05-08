#include "../ext/tap.c/tap.c"

#include "mIterator.c"

const int limit = 2;
int errfn_called = 0;

mit_status_t errfn(void *ctx, void **result) {
  (void)ctx;
  (void)result;
  ++errfn_called;
  return MIT_ERROR;
}

mit_status_t nextfn(void *ctx, void **result) {
  int *c = ctx;
  if (*c >= limit) { return MIT_EXHAUSTED; }
  else { ++(*c); *result = c; return MIT_OK; }
}

int main(void) {
  int ctx = 0;
  mit_result_t *res;
  mit_t *mit = mit_new(nextfn, &ctx, NULL);

  tap_plan(49);

  /**************************
   * successful
   *************************/

  /* item 1 */
  tap_ok((res = mit_peek(mit)) != NULL, "peek(1) returns a result");
  tap_is_int(res->status, MIT_OK, "peek(1) returns OK");
  tap_ok(res->value == &ctx, "peek(1) returns correct pointer");
  tap_is_int(ctx, 1, "peek(1) value is correct");
  tap_is_int(mit_status(mit), MIT_OK, "iterator status is OK");

  tap_ok((res = mit_next(mit)) != NULL, "next(1) returns a result");
  tap_is_int(res->status, MIT_OK, "next(1) returns OK");
  tap_ok(res->value == &ctx, "next(1) returns correct pointer");
  tap_is_int(ctx, 1, "next(1) value is correct");
  tap_is_int(mit_status(mit), MIT_OK, "iterator status is OK");

  /* item 2 */
  tap_ok((res = mit_peek(mit)) != NULL, "peek(2) returns a result");
  tap_is_int(res->status, MIT_OK, "peek(2) returns OK");
  tap_ok(res->value == &ctx, "peek(2) returns correct pointer");
  tap_is_int(ctx, 2, "peek(2) value is correct");
  tap_is_int(mit_status(mit), MIT_OK, "iterator status is OK");

  tap_ok((res = mit_next(mit)) != NULL, "next(2) returns a result");
  tap_is_int(res->status, MIT_OK, "next(2) returns OK");
  tap_ok(res->value == &ctx, "next(2) returns correct pointer");
  tap_is_int(ctx, 2, "next(2) value is correct");
  tap_is_int(mit_status(mit), MIT_OK, "iterator status is OK");

  /* exhausted */
  tap_ok((res = mit_peek(mit)) != NULL, "peek(3) returns a result");
  tap_is_int(res->status, MIT_EXHAUSTED, "peek(3) returns EXHAUSTED");
  tap_ok(res->value == NULL, "peek(3) returns NULL");
  tap_is_int(mit_status(mit), MIT_OK, "peek does not set EXHAUSTED status");

  tap_ok((res = mit_next(mit)) != NULL, "next(3) returns a result");
  tap_is_int(res->status, MIT_EXHAUSTED, "next(3) returns EXHAUSTED");
  tap_ok(res->value == NULL, "next(3) returns NULL ");
  tap_is_int(mit_status(mit), MIT_EXHAUSTED, "iterator status is EXHAUSTED");

  tap_ok((res = mit_next(mit)) != NULL, "next(4) returns a result");
  tap_is_int(res->status, MIT_EXHAUSTED, "next(4) returns EXHAUSTED");
  tap_ok(res->value == NULL, "next(4) returns NULL ");
  tap_is_int(mit_status(mit), MIT_EXHAUSTED, "iterator status is EXHAUSTED");

  mit_free(mit);

  /**************************
   * error
   *************************/

  mit = mit_new(errfn, NULL, NULL);

  /* first call */
  tap_ok((res = mit_peek(mit)) != NULL, "peek(1) returns a result");
  tap_is_int(res->status, MIT_ERROR, "peek(1) returns ERROR");
  tap_ok(res->value == NULL, "peek(1) returns NULL value");
  tap_is_int(mit_status(mit), MIT_ERROR, "peek sets ERROR status");

  tap_ok((res = mit_next(mit)) != NULL, "next(1) returns a result");
  tap_is_int(res->status, MIT_ERROR, "next(1) returns ERROR");
  tap_ok(res->value == NULL, "next(1) returns NULL value");
  tap_is_int(mit_status(mit), MIT_ERROR, "ERROR status set");

  /* second call */
  tap_ok((res = mit_peek(mit)) != NULL, "peek(2) returns a result");
  tap_is_int(res->status, MIT_ERROR, "peek(2) returns ERROR");
  tap_ok(res->value == NULL, "peek(2) returns NULL value");
  tap_is_int(mit_status(mit), MIT_ERROR, "ERROR status set");

  tap_ok((res = mit_next(mit)) != NULL, "next(2) returns a result");
  tap_is_int(res->status, MIT_ERROR, "next(2) returns ERROR");
  tap_ok(res->value == NULL, "next(2) returns NULL value");
  tap_is_int(mit_status(mit), MIT_ERROR, "error status set");

  tap_is_int(errfn_called, 1, "nextfn not called again after error");

  mit_free(mit);

  return tap_finish();
}
