/* Length-controlled RVV libmvec entries (downstream extension).

   These are not the standard RISC-V vector-function ABI.  Each entry takes
   an explicit active-lane count in the first scalar argument (a0), the
   predicate in v0, and the data/result in v8 (m2: v8-v9).  The callee sets
   vl from the length; the incoming vl CSR is not part of the contract.
   Unlike the fixed/length-0 masked entries, length 0 performs no FP work.

   Copyright (C) 2026 Free Software Foundation, Inc.
   This file is part of the GNU C Library.

   The GNU C Library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public License as
   published by the Free Software Foundation; either version 2.1 of the
   License, or (at your option) any later version.  */

#ifndef _RISCV_MATHVEC_LEN_H
#define _RISCV_MATHVEC_LEN_H 1

/* Explicit opt-in for the private length-first masked ABI.  Include this
   header before <math.h> so the declarations in bits/math-vector.h can
   carry the riscv_simd_math_abi_len marker.  The installed public header
   never defines these macros; a compiler with a new header can therefore
   not synthesize _len calls for an old runtime.  */
#define __GLIBC_RISCV_MATHVEC_LEN_LOG 1
#define __GLIBC_RISCV_MATHVEC_LEN_EXP 1

#include <stddef.h>
#include <riscv_vector.h>

#ifdef __cplusplus
extern "C" {
#endif

extern vfloat64m1_t _ZGV1Mxv_log_len (size_t, vbool64_t, vfloat64m1_t);
extern vfloat64m1_t _ZGV1Mxv_exp_len (size_t, vbool64_t, vfloat64m1_t);
extern vfloat64m2_t _ZGV2Mxv_log_len (size_t, vbool32_t, vfloat64m2_t);
extern vfloat64m2_t _ZGV2Mxv_exp_len (size_t, vbool32_t, vfloat64m2_t);

static inline vfloat64m1_t
__riscv_v_log_len_m1 (vfloat64m1_t __x, size_t __n, vbool64_t __m)
{
  return _ZGV1Mxv_log_len (__n, __m, __x);
}

static inline vfloat64m1_t
__riscv_v_exp_len_m1 (vfloat64m1_t __x, size_t __n, vbool64_t __m)
{
  return _ZGV1Mxv_exp_len (__n, __m, __x);
}

static inline vfloat64m2_t
__riscv_v_log_len_m2 (vfloat64m2_t __x, size_t __n, vbool32_t __m)
{
  return _ZGV2Mxv_log_len (__n, __m, __x);
}

static inline vfloat64m2_t
__riscv_v_exp_len_m2 (vfloat64m2_t __x, size_t __n, vbool32_t __m)
{
  return _ZGV2Mxv_exp_len (__n, __m, __x);
}

#ifdef __cplusplus
}
#endif

#endif
