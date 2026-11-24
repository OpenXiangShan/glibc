/* RVV log for LMUL 8.
   Copyright (C) 2026 Free Software Foundation, Inc.
   This file is part of the GNU C Library.
   SPDX-License-Identifier: LGPL-2.1-or-later */

#define RVV_LMUL 8
#define RVV_MASK_BITS 8
#define RVV_BINARY 0
#include "f64.h"
#include "log_data.h"

#define CORE rvv_log_core
#define GATHER RVV_OP (vluxei64_v, FT)
#define RVV_LOG_HOT_LIFETIME 1
#include "log_inline.h"
#include "f64-entry.h"

vfloat64m8_t
_ZGV8N16v_log (vfloat64m8_t x)
{
  return rvv_kernel (x, 16);
}

vfloat64m8_t
_ZGV8Nxv_log (vfloat64m8_t x)
{
  return rvv_kernel (x, SETVLMAX ());
}
