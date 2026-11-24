/* RVV exp for LMUL 2.
   Copyright (C) 2026 Free Software Foundation, Inc.
   This file is part of the GNU C Library.
   SPDX-License-Identifier: LGPL-2.1-or-later */

#define RVV_LMUL 2
#define RVV_MASK_BITS 32
#define RVV_BINARY 0
#include "f64.h"
#include "exp_data.h"

#define CORE rvv_exp_core
#define GATHER UGATHER
#define FU REINTERPRET_FU
#define UF REINTERPRET_UF
#define ULT UCMPLT
#define RVV_EXP_COLD_DISPATCH 1
#define RVV_EXP_HOT_LIFETIME 1
#include "exp_inline.h"
#include "f64-entry.h"

static inline vfloat64m2_t
rvv_kernel_masked (vbool32_t mask, vfloat64m2_t x, size_t vl)
{
  x = rvv_mask_input (mask, x, 0, vl);
  return rvv_kernel (x, vl);
}

vfloat64m2_t
_ZGV2N4v_exp (vfloat64m2_t x)
{
  return rvv_kernel (x, 4);
}

vfloat64m2_t
_ZGV2Nxv_exp (vfloat64m2_t x)
{
  return rvv_kernel (x, SETVLMAX ());
}

vfloat64m2_t
_ZGV2M4v_exp (vbool32_t mask, vfloat64m2_t x)
{
  return rvv_kernel_masked (mask, x, 4);
}

vfloat64m2_t
_ZGV2Mxv_exp (vbool32_t mask, vfloat64m2_t x)
{
  return rvv_kernel_masked (mask, x, SETVLMAX ());
}

/* Private length-controlled ABI.  N <= VLMAX and N == 0 does no FP work. */
vfloat64m2_t
_ZGV2Mxv_exp_len (size_t n, vbool32_t mask, vfloat64m2_t x)
{
  if (n == 0)
    return x;
  return rvv_kernel_masked (mask, x, SETVL (n));
}
