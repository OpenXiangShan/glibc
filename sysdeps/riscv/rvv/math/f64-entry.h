/* Entry helper for an RVV binary64 core.
   Copyright (C) 2026 Free Software Foundation, Inc.
   This file is part of the GNU C Library.
   SPDX-License-Identifier: LGPL-2.1-or-later */

/* The shift-based reductions require nearest rounding.  Keep accrued fflags
   and restore frm.  The vector asm operands order the inputs and result with
   respect to both CSR writes, including when the core is inlined. */
static inline FV
#if RVV_BINARY
rvv_kernel (FV x, FV y, size_t vl)
#else
rvv_kernel (FV x, size_t vl)
#endif
{
  unsigned long frm;
  __asm__ volatile ("frrm %0" : "=r" (frm) : : "memory");
  if (__glibc_unlikely (frm != 0))
    {
#if RVV_BINARY
      __asm__ volatile ("fsrmi 0" : "+vr" (x), "+vr" (y) : : "memory");
#else
      __asm__ volatile ("fsrmi 0" : "+vr" (x) : : "memory");
#endif
    }
#if RVV_BINARY
  FV result = CORE (x, y, vl);
#else
  FV result = CORE (x, vl);
#endif
  if (__glibc_unlikely (frm != 0))
    __asm__ volatile ("fsrm %1" : "+vr" (result) : "r" (frm) : "memory");
  return result;
}
