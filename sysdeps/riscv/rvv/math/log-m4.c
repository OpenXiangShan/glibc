/* RVV log for LMUL 4.
   Copyright (C) 2026 Free Software Foundation, Inc.
   This file is part of the GNU C Library.
   SPDX-License-Identifier: LGPL-2.1-or-later */

#define RVV_LMUL 4
#define RVV_MASK_BITS 16
#define RVV_BINARY 0
#include "f64.h"
#include "log_data.h"

#define CORE rvv_log_core
#define GATHER RVV_OP (vluxei64_v, FT)
#define RVV_LOG_HOT_LIFETIME 1
#include "log_inline.h"
#include "f64-entry.h"

vfloat64m4_t
_ZGV4N8v_log (vfloat64m4_t x)
{
  return rvv_kernel (x, 8);
}

vfloat64m4_t
_ZGV4Nxv_log (vfloat64m4_t x)
{
  return rvv_kernel (x, SETVLMAX ());
}
