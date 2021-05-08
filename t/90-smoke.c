#include <inttypes.h>

#include "../ext/tap.c/tap.c"

#include "mIterator.c"

/* fully consume a large iterator to check performance */

const uint32_t limit = 50 * 1000 * 1000;

mit_status_t nextfn(void *ctx, void **result) {
  uint32_t *c = ctx;
  if (*c >= limit) { return MIT_EXHAUSTED; }
  else { ++(*c); *result = c; return MIT_OK; }
}

int main(void) {
  uint32_t ctx = 0, count = 0;
  mit_t *mit = mit_new(nextfn, &ctx, NULL);

  tap_plan(3);

  /* consume the entire iterator */
  while (mit_next(mit)->status == MIT_OK) { ++count; }

  tap_is_int(mit_status(mit), MIT_EXHAUSTED, "iterator indicates exhaustion");
  tap_is_int(count, limit, "iterator actually exhausted");
  tap_is_int(ctx, limit, "final value reached %" PRIu32, limit);

  mit_free(mit);

  return tap_finish();
}
