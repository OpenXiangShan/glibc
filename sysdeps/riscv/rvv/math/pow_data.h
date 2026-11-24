/* Data for the log part of pow.
   Copyright (C) 2018-2026 Free Software Foundation, Inc.
   This file is part of the GNU C Library.

   The GNU C Library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   The GNU C Library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with the GNU C Library; if not, see
   <https://www.gnu.org/licenses/>.  */
/* Private data copied without numeric changes from e_pow_log_data.c and
   e_exp_data.c, glibc 2018-2026. No public or hidden data symbol is exported. */
#ifndef EXPERIMENTAL_POWVEC_DATA_H
#define EXPERIMENTAL_POWVEC_DATA_H
#include <stdint.h>
struct powvec_log_table
{
    double ln2hi, ln2lo, poly[7];
    struct { double invc, pad, logc, logctail; } tab[128];
};
extern const struct powvec_log_table powvec_log_data
  __attribute__ ((visibility ("hidden")));
struct powvec_exp_table
{
    double invln2N, negln2hiN, negln2loN, shift, poly[4];
    uint64_t tab[256];
};
extern const struct powvec_exp_table powvec_exp_data
  __attribute__ ((visibility ("hidden")));
#endif
