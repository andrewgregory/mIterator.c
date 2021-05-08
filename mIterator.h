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

#ifndef MITERATOR_H
#define MITERATOR_H

#include <limits.h>
#include <stdio.h>

typedef struct mit_t mit_t;

typedef enum mit_status_t {
  MIT_OK = 0,
  MIT_ERROR,
  MIT_EXHAUSTED
} mit_status_t;

typedef struct mit_result_t {
  mit_status_t status;
  void *value;
} mit_result_t;

typedef mit_status_t (*mit_next_fn_t)(void *ctx, void **result);
typedef mit_status_t (*mit_grep_fn_t)(void *value, void *ctx, int *matches);
typedef mit_status_t (*mit_map_fn_t)(void *value, void *ctx, void **result);
typedef void         (*mit_free_fn_t)(void *ctx);

mit_t *mit_new(mit_next_fn_t next, void *ctx, mit_free_fn_t freefn);
mit_t *mit_finite_new(mit_next_fn_t next, void *ctx, mit_free_fn_t freefn);
mit_t *mit_grep(mit_t *mit, mit_grep_fn_t fn, void *ctx, mit_free_fn_t freefn);
mit_t *mit_map(mit_t *mit, mit_map_fn_t fn, void *ctx, mit_free_fn_t freefn);
mit_t *mit_chain(mit_t *mit1, mit_t *mit2);
void   mit_free(mit_t *mit);

mit_result_t *mit_next(mit_t *mit);
mit_result_t *mit_peek(mit_t *mit);
mit_result_t *mit_nth(mit_t *mit, size_t n);
mit_status_t  mit_skip(mit_t *mit, size_t n);

mit_status_t mit_status(mit_t *mit);
int mit_is_ready(mit_t *mit);
int mit_is_error(mit_t *mit);
int mit_is_exhausted(mit_t *mit);
int mit_is_finite(mit_t *mit);

void *mit_ctx(mit_t *mit);

#endif /* MITERATOR_H */

/* vim: set ts=2 sw=2 et: */
