/* Constant data for the RVV exp kernel.
   Copyright (C) 2017-2026 Free Software Foundation, Inc.
   SPDX-License-Identifier: LGPL-2.1-or-later. */
#ifndef SCALAR_TABLES_H
#define SCALAR_TABLES_H
#include <stdint.h>
struct expvec_data {
 double invln2N,negln2hiN,negln2loN,poly[4],shift;
 uint64_t tab[256];
};
extern const struct expvec_data expvec_data
  __attribute__ ((visibility ("hidden")));
#endif
