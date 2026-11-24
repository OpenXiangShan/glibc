/* RVV sin for LMUL 8.
   Copyright (C) 2026 Free Software Foundation, Inc.
   This file is part of the GNU C Library.
   SPDX-License-Identifier: LGPL-2.1-or-later */

#define RVV_LMUL 8
#define RVV_MASK_BITS 8
#define RVV_BINARY 0
#include "f64.h"
#include "sincos_data.h"

#define CORE rvv_sin_core
#define GATHER RVV_OP (vluxei64_v, FT)
#define TRIG_COS 0
#define SCALAR sin
#include "sincos_inline.h"
#include "f64-entry.h"

vfloat64m8_t
_ZGV8N16v_sin (vfloat64m8_t x)
{
  return rvv_kernel (x, 16);
}

vfloat64m8_t
_ZGV8Nxv_sin (vfloat64m8_t x)
{
  return rvv_kernel (x, SETVLMAX ());
}
