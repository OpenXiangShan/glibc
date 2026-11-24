/* RVV pow for LMUL 8.
   Copyright (C) 2026 Free Software Foundation, Inc.
   This file is part of the GNU C Library.
   SPDX-License-Identifier: LGPL-2.1-or-later */

#define RVV_LMUL 8
#define RVV_MASK_BITS 8
#define RVV_BINARY 1
#include "f64.h"
#include "pow_data.h"

#define CORE rvv_pow_core
#define GATHER RVV_OP (vluxei64_v, FT)
#define RVV_POW_COLD_DISPATCH 1
#define RVV_POW_HOT_LIFETIME 1
#include "pow_inline.h"
#include "f64-entry.h"

vfloat64m8_t
_ZGV8N16vv_pow (vfloat64m8_t x, vfloat64m8_t y)
{
  return rvv_kernel (x, y, 16);
}

vfloat64m8_t
_ZGV8Nxvv_pow (vfloat64m8_t x, vfloat64m8_t y)
{
  return rvv_kernel (x, y, SETVLMAX ());
}
