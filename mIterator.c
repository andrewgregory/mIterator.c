/*
 * Copyright 2021 Andrew Gregory <andrew.gregory.8@gmail.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 *
 * Project URL: https://github.com/andrewgregory/mIterator.c
 */

#ifndef MITERATOR_C
#define MITERATOR_C

#include <stdlib.h>

#include "mIterator.h"

struct mit_t {
  int finite;
  mit_status_t status;

  mit_result_t value;
  mit_result_t next;  /* cached peek value */
  int next_set;       /* is the next value cached */

  void *ctx;
  mit_free_fn_t freefn;
  mit_next_fn_t nextfn;
};

mit_t *mit_new(mit_next_fn_t nextfn, void *ctx, mit_free_fn_t freefn) {
  mit_t *mit = calloc(1, sizeof(mit_t));
  if (mit != NULL) {
    mit->ctx = ctx;
    mit->nextfn = nextfn;
    mit->freefn = freefn;
  }
  return mit;
}

mit_t *mit_finite_new(mit_next_fn_t nextfn, void *ctx, mit_free_fn_t freefn) {
  mit_t *mit = mit_new(nextfn, ctx, freefn);
  if (mit != NULL) {
    mit->finite = 1;
  }
  return mit;
}

void mit_free(mit_t *mit) {
  if (mit) {
    if (mit->freefn) {
      mit->freefn(mit->ctx);
    }
    free(mit);
  }
}

mit_result_t *mit_peek(mit_t *mit) {
  /* ensure result is always a valid pointer */
  if (!mit_is_ready(mit) || mit->next_set) {
    return &mit->value;
  } else {
    switch ((mit->value.status = mit->nextfn(mit->ctx, &mit->value.value))) {
      case MIT_EXHAUSTED:
        mit->value.value = NULL;
      /* fall through */
      case MIT_OK:
        mit->next_set = 1;
        return &mit->value;
      default:
        mit->status = mit->value.status = MIT_ERROR;
        mit->value.value = NULL;
        return &mit->value;
    }
  }
}

mit_result_t *mit_next(mit_t *mit) {
  mit_peek(mit);
  mit->status = mit->value.status;
  mit->next_set = 0; /* indicate the value has been consumed */
  return &mit->value;
}

mit_status_t mit_skip(mit_t *mit, size_t n) {
  while (n-- && mit_next(mit)->status == MIT_OK);
  return mit_status(mit);
}

mit_result_t *mit_nth(mit_t *mit, size_t n) {
  mit_skip(mit, n);
  return mit_next(mit);
}

mit_status_t mit_status(mit_t *mit) {
  return mit->status;
}

int mit_is_ready(mit_t *mit) {
  return mit->status == MIT_OK;
}

int mit_is_exhausted(mit_t *mit) {
  return mit->status == MIT_EXHAUSTED;
}

int mit_is_error(mit_t *mit) {
  return mit->status == MIT_ERROR;
}

int mit_is_finite(mit_t *mit) {
  return mit->finite;
}

void *mit_ctx(mit_t *mit) {
  return mit->ctx;
}

/*****************
 * grep iterator *
 ****************/

struct _mit_grep_ctx_t {
  mit_t *mit;
  void *ctx;
  mit_free_fn_t freefn;
  mit_grep_fn_t grepfn;
};

static void _mit_grep_free(struct _mit_grep_ctx_t *ctx) {
  if (ctx) {
    if (ctx->freefn) {
      ctx->freefn(ctx->ctx);
    }
    mit_free(ctx->mit);
    free(ctx);
  }
}

static mit_status_t _mit_grep_next(void *ctx, void **result) {
  struct _mit_grep_ctx_t *gctx = ctx;
  mit_result_t *res;
  while ((res = mit_next(gctx->mit))->status == MIT_OK) {
    int matches = 0;
    switch (gctx->grepfn(res->value, gctx->ctx, &matches)) {
      case MIT_OK:
        if (matches) {
          *result = res->value;
          return MIT_OK;
        }
        break;
      default:
        return MIT_ERROR;
    }
  }
  return res->status;
}

mit_t *mit_grep(mit_t *mit, mit_grep_fn_t grepfn,
    void *ctx, mit_free_fn_t freefn) {
  struct _mit_grep_ctx_t *gctx;
  mit_t *new;

  if (!(gctx = calloc(1, sizeof(struct _mit_grep_ctx_t)))) { return NULL; }
  if (!(new = mit_new(_mit_grep_next, gctx, (mit_free_fn_t) _mit_grep_free))) {
    _mit_grep_free(gctx);
    return NULL;
  }

  gctx->mit = mit;
  gctx->ctx = ctx;
  gctx->grepfn = grepfn;
  gctx->freefn = freefn;

  new->finite = mit->finite;

  return new;
}

/****************
 * map iterator *
 ***************/

struct _mit_map_ctx_t {
  mit_t *mit;
  void *ctx;
  mit_free_fn_t freefn;
  mit_map_fn_t mapfn;
};

static void _mit_map_free(struct _mit_map_ctx_t *ctx) {
  if (ctx) {
    if (ctx->freefn) {
      ctx->freefn(ctx->ctx);
    }
    mit_free(ctx->mit);
    free(ctx);
  }
}

static mit_status_t _mit_map_next(void *ctx, void **result) {
  struct _mit_map_ctx_t *mctx = ctx;
  mit_result_t *res;
  switch ((res = mit_next(mctx->mit))->status) {
    case MIT_OK:
      return mctx->mapfn(res->value, mctx->ctx, result) == MIT_OK
          ? MIT_OK : MIT_ERROR;
    default:
      return mit_status(mctx->mit);
  }
}

mit_t *mit_map(mit_t *mit, mit_map_fn_t mapfn,
    void *ctx, mit_free_fn_t freefn) {
  struct _mit_map_ctx_t *mctx;
  mit_t *new;

  if (!(mctx = calloc(1, sizeof(struct _mit_map_ctx_t)))) { return NULL; }
  if (!(new = mit_new(_mit_map_next, mctx, (mit_free_fn_t) _mit_map_free))) {
    _mit_map_free(mctx);
    return NULL;
  }

  mctx->mit = mit;
  mctx->ctx = ctx;
  mctx->mapfn = mapfn;
  mctx->freefn = freefn;

  new->finite = mit->finite;

  return new;
}

/******************
 * chain iterator *
 *****************/

struct _mit_chain_ctx_t {
  mit_t *mit1;
  mit_t *mit2;
};

static void _mit_chain_free(struct _mit_chain_ctx_t *ctx) {
  if (ctx) {
    mit_free(ctx->mit1);
    mit_free(ctx->mit2);
    free(ctx);
  }
}

static mit_status_t _mit_chain_next(void *ctx, void **result) {
  struct _mit_chain_ctx_t *cctx = ctx;
  mit_result_t *res;
  if (cctx->mit1 == NULL) { return MIT_EXHAUSTED; }
  switch ((res = mit_next(cctx->mit1))->status) {
    case MIT_OK:
      *result = res->value;
      return MIT_OK;
    case MIT_EXHAUSTED:
      mit_free(cctx->mit1);
      cctx->mit1 = cctx->mit2;
      cctx->mit2 = NULL;
      return _mit_chain_next(ctx, result);
    default:
      return MIT_ERROR;
  }
}

mit_t *mit_chain(mit_t *mit1, mit_t *mit2) {
  struct _mit_chain_ctx_t *cctx;
  mit_t *new;

  if (!(cctx = calloc(1, sizeof(struct _mit_chain_ctx_t)))) { return NULL; }
  if (!(new = mit_new(_mit_chain_next, cctx, (mit_free_fn_t) _mit_chain_free))) {
    _mit_chain_free(cctx);
    return NULL;
  }

  cctx->mit1 = mit1;
  cctx->mit2 = mit2;

  new->finite = mit1->finite && mit2->finite;

  return new;
}

#endif /* MITERATOR_C */

/* vim: set ts=2 sw=2 et: */
