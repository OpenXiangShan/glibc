/* RVV sin for LMUL 2.
   Copyright (C) 2026 Free Software Foundation, Inc.
   This file is part of the GNU C Library.
   SPDX-License-Identifier: LGPL-2.1-or-later */

#define RVV_LMUL 2
#define RVV_MASK_BITS 32
#define RVV_BINARY 0
#include "f64.h"
#include "sincos_data.h"

#define CORE rvv_sin_core
#define GATHER RVV_OP (vluxei64_v, FT)
#define TRIG_COS 0
#define SCALAR sin
#include "sincos_inline.h"
#include "f64-entry.h"

static inline vfloat64m2_t
rvv_kernel_masked (vbool32_t mask, vfloat64m2_t x, size_t vl)
{
  x = rvv_mask_input (mask, x, 0, vl);
  return rvv_kernel (x, vl);
}

vfloat64m2_t
_ZGV2N4v_sin (vfloat64m2_t x)
{
  return rvv_kernel (x, 4);
}

vfloat64m2_t
_ZGV2Nxv_sin (vfloat64m2_t x)
{
  return rvv_kernel (x, SETVLMAX ());
}

vfloat64m2_t
_ZGV2M4v_sin (vbool32_t mask, vfloat64m2_t x)
{
  return rvv_kernel_masked (mask, x, 4);
}

vfloat64m2_t
_ZGV2Mxv_sin (vbool32_t mask, vfloat64m2_t x)
{
  return rvv_kernel_masked (mask, x, SETVLMAX ());
}
