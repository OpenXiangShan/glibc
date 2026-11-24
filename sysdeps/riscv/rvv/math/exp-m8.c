/* RVV exp for LMUL 8.
   Copyright (C) 2026 Free Software Foundation, Inc.
   This file is part of the GNU C Library.
   SPDX-License-Identifier: LGPL-2.1-or-later */

#define RVV_LMUL 8
#define RVV_MASK_BITS 8
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

vfloat64m8_t
_ZGV8N16v_exp (vfloat64m8_t x)
{
  return rvv_kernel (x, 16);
}

vfloat64m8_t
_ZGV8Nxv_exp (vfloat64m8_t x)
{
  return rvv_kernel (x, SETVLMAX ());
}
