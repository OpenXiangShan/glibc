/* RVV log for LMUL 1.
   Copyright (C) 2026 Free Software Foundation, Inc.
   This file is part of the GNU C Library.
   SPDX-License-Identifier: LGPL-2.1-or-later */

#define RVV_LMUL 1
#define RVV_MASK_BITS 64
#define RVV_BINARY 0
#include "f64.h"
#include "log_data.h"

#define CORE rvv_log_core
#define GATHER RVV_OP (vluxei64_v, FT)
#define RVV_LOG_HOT_LIFETIME 1
#define RVV_LOG_NO_BARRIER 1
#define RVV_LOG_USE_MACC 1
#include "log_inline.h"
#include "f64-entry.h"

static inline vfloat64m1_t
rvv_kernel_masked (vbool64_t mask, vfloat64m1_t x, size_t vl)
{
  x = rvv_mask_input (mask, x, UINT64_C (0x3ff0000000000000), vl);
  return rvv_kernel (x, vl);
}

vfloat64m1_t
_ZGV1N2v_log (vfloat64m1_t x)
{
  return rvv_kernel (x, 2);
}

vfloat64m1_t
_ZGV1Nxv_log (vfloat64m1_t x)
{
  return rvv_kernel (x, SETVLMAX ());
}

vfloat64m1_t
_ZGV1M2v_log (vbool64_t mask, vfloat64m1_t x)
{
  return rvv_kernel_masked (mask, x, 2);
}

vfloat64m1_t
_ZGV1Mxv_log (vbool64_t mask, vfloat64m1_t x)
{
  return rvv_kernel_masked (mask, x, SETVLMAX ());
}

/* Private length-controlled ABI.  N <= VLMAX and N == 0 does no FP work. */
vfloat64m1_t
_ZGV1Mxv_log_len (size_t n, vbool64_t mask, vfloat64m1_t x)
{
  if (n == 0)
    return x;
  return rvv_kernel_masked (mask, x, SETVL (n));
}
