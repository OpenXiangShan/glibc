/* RVV pow for LMUL 1.
   Copyright (C) 2026 Free Software Foundation, Inc.
   This file is part of the GNU C Library.
   SPDX-License-Identifier: LGPL-2.1-or-later */

#define RVV_LMUL 1
#define RVV_MASK_BITS 64
#define RVV_BINARY 1
#include "f64.h"
#include "pow_data.h"

#define CORE rvv_pow_core
#define GATHER RVV_OP (vluxei64_v, FT)
#define RVV_POW_COLD_DISPATCH 1
#define RVV_POW_HOT_LIFETIME 1
#include "pow_inline.h"
#include "f64-entry.h"

static inline vfloat64m1_t
rvv_kernel_masked (vbool64_t mask, vfloat64m1_t x, vfloat64m1_t y, size_t vl)
{
  x = rvv_mask_input (mask, x, UINT64_C (0x3ff0000000000000), vl);
  y = rvv_mask_input (mask, y, UINT64_C (0x3ff0000000000000), vl);
  return rvv_kernel (x, y, vl);
}

vfloat64m1_t
_ZGV1N2vv_pow (vfloat64m1_t x, vfloat64m1_t y)
{
  return rvv_kernel (x, y, 2);
}

vfloat64m1_t
_ZGV1Nxvv_pow (vfloat64m1_t x, vfloat64m1_t y)
{
  return rvv_kernel (x, y, SETVLMAX ());
}

vfloat64m1_t
_ZGV1M2vv_pow (vbool64_t mask, vfloat64m1_t x, vfloat64m1_t y)
{
  return rvv_kernel_masked (mask, x, y, 2);
}

vfloat64m1_t
_ZGV1Mxvv_pow (vbool64_t mask, vfloat64m1_t x, vfloat64m1_t y)
{
  return rvv_kernel_masked (mask, x, y, SETVLMAX ());
}
