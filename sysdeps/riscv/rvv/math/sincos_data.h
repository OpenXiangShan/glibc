/* IBM Accurate Mathematical Library table, from GNU libc sincostab.c.
   Copyright (C) 2001-2026 Free Software Foundation, Inc.
   SPDX-License-Identifier: LGPL-2.1-or-later */
#ifndef TRIGVEC_DATA_H
#define TRIGVEC_DATA_H
#include <stddef.h>
/* The original 110 records: sin_hi, sin_lo, cos_hi, cos_lo at i/128. */
extern const double trigvec_sincostab[440]
  __attribute__ ((visibility ("hidden")));
#endif
