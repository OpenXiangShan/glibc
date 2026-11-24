/* RVV pow for LMUL 2.
   Copyright (C) 2026 Free Software Foundation, Inc.
   This file is part of the GNU C Library.
   SPDX-License-Identifier: LGPL-2.1-or-later */

#define RVV_LMUL 2
#define RVV_MASK_BITS 32
#define RVV_BINARY 1
#include "f64.h"
#include "pow_data.h"

#define CORE rvv_pow_core
#define GATHER RVV_OP (vluxei64_v, FT)
#define RVV_POW_COLD_DISPATCH 1
#define RVV_POW_HOT_LIFETIME 1
#include "pow_inline.h"
#include "f64-entry.h"

static inline vfloat64m2_t
rvv_kernel_masked (vbool32_t mask, vfloat64m2_t x, vfloat64m2_t y, size_t vl)
{
  x = rvv_mask_input (mask, x, UINT64_C (0x3ff0000000000000), vl);
  y = rvv_mask_input (mask, y, UINT64_C (0x3ff0000000000000), vl);
  return rvv_kernel (x, y, vl);
}

vfloat64m2_t
_ZGV2N4vv_pow (vfloat64m2_t x, vfloat64m2_t y)
{
  return rvv_kernel (x, y, 4);
}

vfloat64m2_t
_ZGV2Nxvv_pow (vfloat64m2_t x, vfloat64m2_t y)
{
  return rvv_kernel (x, y, SETVLMAX ());
}

vfloat64m2_t
_ZGV2M4vv_pow (vbool32_t mask, vfloat64m2_t x, vfloat64m2_t y)
{
  return rvv_kernel_masked (mask, x, y, 4);
}

vfloat64m2_t
_ZGV2Mxvv_pow (vbool32_t mask, vfloat64m2_t x, vfloat64m2_t y)
{
  return rvv_kernel_masked (mask, x, y, SETVLMAX ());
}
