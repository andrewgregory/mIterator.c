#include "../ext/tap.c/tap.c"

#include "mIterator.c"

const int limit = 2;
int mit1_freed = 0;
int mit2_freed = 0;

void freefn1(void *v) {
  (void)v;
  ++mit1_freed;
}

void freefn2(void *v) {
  (void)v;
  ++mit2_freed;
}

mit_status_t nextfn(void *ctx, void **result) {
  int *c = ctx;
  if (*c >= limit) { return MIT_EXHAUSTED; }
  else { ++(*c); *result = c; return MIT_OK; }
}

int main(void) {
  mit_result_t *res;
  int ctx1 = 0, ctx2 = 1;
  mit_t *mit1 = mit_new(nextfn, &ctx1, freefn1);
  mit_t *mit2 = mit_new(nextfn, &ctx2, freefn2);
  mit_t *chain = mit_chain(mit1, mit2);

  tap_plan(17);

  /* item 1 */
  tap_ok((res = mit_next(chain)) != NULL, "next(1) returns a result");
  tap_is_int(res->status, MIT_OK, "next(1) returns OK");
  tap_is_int(*((int *) res->value), 1, "next(1) returns correct value");

  /* item 2 */
  tap_ok((res = mit_next(chain)) != NULL, "next(2) returns a result");
  tap_is_int(res->status, MIT_OK, "next(2) returns OK");
  tap_is_int(*((int *) res->value), 2, "next(2) returns correct value");

  /* item 3 */
  tap_ok((res = mit_next(chain)) != NULL, "next(3) returns a result");
  tap_is_int(res->status, MIT_OK, "next(3) returns OK");
  tap_is_int(*((int *) res->value), 2, "next(3) returns correct value");
  tap_is_int(mit1_freed, 1, "first inner iterator freed at exhaustion");

  /* exhausted */
  tap_ok((res = mit_next(chain)) != NULL, "next(4) returns a result");
  tap_is_int(res->status, MIT_EXHAUSTED, "next(4) returns EXHAUSTED");
  tap_ok(res->value == NULL, "next(4) returns NULL ");
  tap_is_int(mit_status(chain), MIT_EXHAUSTED, "iterator status is EXHAUSTED");
  tap_is_int(mit2_freed, 1, "second inner iterator freed at exhaustion");

  mit_free(chain);

  tap_is_int(mit1_freed, 1, "first inner iterator freed");
  tap_is_int(mit2_freed, 1, "second inner iterator freed");

  return tap_finish();
}
