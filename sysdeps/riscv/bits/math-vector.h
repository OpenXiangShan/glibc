/* Downstream RISC-V SIMD math declarations.
   Copyright (C) 2026 Free Software Foundation, Inc.
   This file is part of the GNU C Library.

   The GNU C Library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public License as
   published by the Free Software Foundation; either version 2.1 of the
   License, or (at your option) any later version.  */
#ifndef _MATH_H
# error "Never include <bits/math-vector.h> directly; include <math.h> instead."
#endif

#include <bits/libm-simd-decl-stubs.h>
/* The lp64d build alone installs this availability header.  It is absent in
   installations containing only other ABIs.  Internal builds do not need
   SIMD declarations and need not find the generated installed header.  */
#if !defined _LIBC && defined __riscv_xlen && __riscv_xlen == 64 \
    && defined __riscv_float_abi_double && defined __has_include
# if __has_include (<bits/riscv-mathvec.h>)
#  include <bits/riscv-mathvec.h>
# endif
#endif
/* New compiler/library handshake.  The per-function masks are generated from
   the exact manifest rows; an old sysroot has no masks and takes the legacy
   m2-only path below.  The compiler attribute may be absent even on a new
   library, in which case the old m2 declarations remain available.  */
#if defined __FAST_MATH__ \
    && defined __riscv_xlen && __riscv_xlen == 64 \
    && defined __riscv_float_abi_double \
    && defined __riscv_vector \
    && defined __riscv_simd_math_abi_v1 \
    && __riscv_simd_math_abi_v1 == 1 \
    && defined __GLIBC_RISCV_MATHVEC_LOG_ABI_MASK
# define __GLIBC_RISCV_MATHVEC_ABI_N_ATTR(mask) \
  __attribute__ ((__simd__ ("notinbranch"), \
                  __riscv_simd_math_abi_v1__ (mask), const))
# define __GLIBC_RISCV_MATHVEC_ABI_M_ATTR(mask) \
  __attribute__ ((__simd__ ("inbranch"), \
                  __riscv_simd_math_abi_v1__ (mask), const))
# define __GLIBC_RISCV_MATHVEC_ABI_NM_ATTR(mask) \
  __attribute__ ((__simd__ ("notinbranch"), __simd__ ("inbranch"), \
                  __riscv_simd_math_abi_v1__ (mask), const))
/* The union bits of every mask the header may hold.  A compiler that predates
   the 16-bit mask rejects an attribute argument above 255 outright, so the
   LMUL4/LMUL8 nibbles are only offered when the compiler advertises
   __riscv_simd_math_abi_v1_lmul8; the matching generated mask macros fall back
   to the historical eight-bit LMUL1/2 subset in the same condition.  */
# if defined __riscv_simd_math_abi_v1_lmul8
#  define __GLIBC_RISCV_MATHVEC_ABI_N_BITS 0x3333
#  define __GLIBC_RISCV_MATHVEC_ABI_M_BITS 0xcccc
# else
#  define __GLIBC_RISCV_MATHVEC_ABI_N_BITS 0x33
#  define __GLIBC_RISCV_MATHVEC_ABI_M_BITS 0xcc
# endif

/* The private length-first masked ABI is never advertised by the installed
   header.  Only a translation unit that defines the private family opt-in
   macros (for example through riscv-mathvec-len.h) and uses a compiler with
   the explicit capability gets the length marker on the declaration.  An
   old header therefore cannot make a new compiler synthesize _len clones
   and break link against an old libmvec.  */
# if defined __riscv_simd_math_abi_len \
     && __riscv_simd_math_abi_len == 1 \
     && defined __GLIBC_RISCV_MATHVEC_LEN_LOG
#  define __GLIBC_RISCV_MATHVEC_LOG_LEN_ATTR \
  __attribute__ ((__riscv_simd_math_abi_len__))
# else
#  define __GLIBC_RISCV_MATHVEC_LOG_LEN_ATTR
# endif
# if defined __riscv_simd_math_abi_len \
     && __riscv_simd_math_abi_len == 1 \
     && defined __GLIBC_RISCV_MATHVEC_LEN_EXP
#  define __GLIBC_RISCV_MATHVEC_EXP_LEN_ATTR \
  __attribute__ ((__riscv_simd_math_abi_len__))
# else
#  define __GLIBC_RISCV_MATHVEC_EXP_LEN_ATTR
# endif

/* Per-function declarations are generated from the same manifest rows as the
   availability masks above, so this header never hand-codes an entry point
   that the installed libmvec does not export.  */
# include <bits/riscv-mathvec-decls.h>

#else /* Legacy m2-only compiler/library handshake.  */

#ifndef __GLIBC_RISCV_MATHVEC
# define __GLIBC_RISCV_MATHVEC 0
#endif

#ifndef __GLIBC_RISCV_MATHVEC_LOG_M2_VLA
# define __GLIBC_RISCV_MATHVEC_LOG_M2_VLA 0
#endif

/* Independent downstream compiler/library capability handshakes: the fixed
   and scalable unmasked log entries need not be present in older sysroots.  */
#if defined __FAST_MATH__ \
    && ((__GLIBC_RISCV_MATHVEC && defined __riscv_simd_math_log_m2 \
         && __riscv_simd_math_log_m2 == 1) \
        || (__GLIBC_RISCV_MATHVEC_LOG_M2_VLA \
            && defined __riscv_simd_math_log_m2_vla \
            && __riscv_simd_math_log_m2_vla == 1))
# undef __DECL_SIMD_log
# define __DECL_SIMD_log __attribute__ ((__simd__ ("notinbranch"), const))
#endif

#ifndef __GLIBC_RISCV_MATHVEC_LOG_M2_MASK
# define __GLIBC_RISCV_MATHVEC_LOG_M2_MASK 0
#endif
#ifndef __GLIBC_RISCV_MATHVEC_LOG_M2_MASK_VLA
# define __GLIBC_RISCV_MATHVEC_LOG_M2_MASK_VLA 0
#endif
#if defined __FAST_MATH__ \
    && ((__GLIBC_RISCV_MATHVEC_LOG_M2_MASK \
         && defined __riscv_simd_math_log_m2_mask \
         && __riscv_simd_math_log_m2_mask == 1) \
        || (__GLIBC_RISCV_MATHVEC_LOG_M2_MASK_VLA \
            && defined __riscv_simd_math_log_m2_mask_vla \
            && __riscv_simd_math_log_m2_mask_vla == 1))
# undef __DECL_SIMD_log
# if ((__GLIBC_RISCV_MATHVEC && defined __riscv_simd_math_log_m2 \
       && __riscv_simd_math_log_m2 == 1) \
      || (__GLIBC_RISCV_MATHVEC_LOG_M2_VLA \
          && defined __riscv_simd_math_log_m2_vla \
          && __riscv_simd_math_log_m2_vla == 1))
#  define __DECL_SIMD_log __attribute__ ((__simd__ ("notinbranch"), \
                                        __simd__ ("inbranch"), const))
# else
#  define __DECL_SIMD_log __attribute__ ((__simd__ ("inbranch"), const))
# endif
#endif

#ifndef __GLIBC_RISCV_MATHVEC_EXP_M2
# define __GLIBC_RISCV_MATHVEC_EXP_M2 0
#endif
#if __GLIBC_RISCV_MATHVEC_EXP_M2 && defined __FAST_MATH__ \
    && ((defined __riscv_simd_math_exp_m2 \
         && __riscv_simd_math_exp_m2 == 1) \
        || (defined __riscv_simd_math_exp_m2_vla \
            && __riscv_simd_math_exp_m2_vla == 1))
# undef __DECL_SIMD_exp
# define __DECL_SIMD_exp __attribute__ ((__simd__ ("notinbranch"), const))
#endif

#ifndef __GLIBC_RISCV_MATHVEC_EXP_M2_MASK
# define __GLIBC_RISCV_MATHVEC_EXP_M2_MASK 0
#endif
#ifndef __GLIBC_RISCV_MATHVEC_EXP_M2_MASK_VLA
# define __GLIBC_RISCV_MATHVEC_EXP_M2_MASK_VLA 0
#endif
#if defined __FAST_MATH__ \
    && ((__GLIBC_RISCV_MATHVEC_EXP_M2_MASK \
         && defined __riscv_simd_math_exp_m2_mask \
         && __riscv_simd_math_exp_m2_mask == 1) \
        || (__GLIBC_RISCV_MATHVEC_EXP_M2_MASK_VLA \
            && defined __riscv_simd_math_exp_m2_mask_vla \
            && __riscv_simd_math_exp_m2_mask_vla == 1))
# undef __DECL_SIMD_exp
# if ((__GLIBC_RISCV_MATHVEC && defined __riscv_simd_math_exp_m2 \
       && __riscv_simd_math_exp_m2 == 1) \
      || (__GLIBC_RISCV_MATHVEC_EXP_M2_VLA \
          && defined __riscv_simd_math_exp_m2_vla \
          && __riscv_simd_math_exp_m2_vla == 1))
#  define __DECL_SIMD_exp __attribute__ ((__simd__ ("notinbranch"), \
                                        __simd__ ("inbranch"), const))
# else
#  define __DECL_SIMD_exp __attribute__ ((__simd__ ("inbranch"), const))
# endif
#endif

#ifndef __GLIBC_RISCV_MATHVEC_SIN_M2
# define __GLIBC_RISCV_MATHVEC_SIN_M2 0
#endif
#if __GLIBC_RISCV_MATHVEC_SIN_M2 && defined __FAST_MATH__ \
    && ((defined __riscv_simd_math_sin_m2 \
         && __riscv_simd_math_sin_m2 == 1) \
        || (defined __riscv_simd_math_sin_m2_vla \
            && __riscv_simd_math_sin_m2_vla == 1))
# undef __DECL_SIMD_sin
# define __DECL_SIMD_sin __attribute__ ((__simd__ ("notinbranch"), const))
#endif

#ifndef __GLIBC_RISCV_MATHVEC_COS_M2
# define __GLIBC_RISCV_MATHVEC_COS_M2 0
#endif
#if __GLIBC_RISCV_MATHVEC_COS_M2 && defined __FAST_MATH__ \
    && ((defined __riscv_simd_math_cos_m2 \
         && __riscv_simd_math_cos_m2 == 1) \
        || (defined __riscv_simd_math_cos_m2_vla \
            && __riscv_simd_math_cos_m2_vla == 1))
# undef __DECL_SIMD_cos
# define __DECL_SIMD_cos __attribute__ ((__simd__ ("notinbranch"), const))
#endif

#ifndef __GLIBC_RISCV_MATHVEC_POW_M2
# define __GLIBC_RISCV_MATHVEC_POW_M2 0
#endif
#if __GLIBC_RISCV_MATHVEC_POW_M2 && defined __FAST_MATH__ \
    && ((defined __riscv_simd_math_pow_m2 \
         && __riscv_simd_math_pow_m2 == 1) \
        || (defined __riscv_simd_math_pow_m2_vla \
            && __riscv_simd_math_pow_m2_vla == 1))
# undef __DECL_SIMD_pow
# define __DECL_SIMD_pow __attribute__ ((__simd__ ("notinbranch"), const))
#endif

#ifndef __GLIBC_RISCV_MATHVEC_LOGF_M2
# define __GLIBC_RISCV_MATHVEC_LOGF_M2 0
#endif
#if __GLIBC_RISCV_MATHVEC_LOGF_M2 && defined __FAST_MATH__ \
    && ((defined __riscv_simd_math_logf_m2 \
         && __riscv_simd_math_logf_m2 == 1) \
        || (defined __riscv_simd_math_logf_m2_vla \
            && __riscv_simd_math_logf_m2_vla == 1))
# undef __DECL_SIMD_logf
# define __DECL_SIMD_logf __attribute__ ((__simd__ ("notinbranch"), const))
#endif

#ifndef __GLIBC_RISCV_MATHVEC_EXPF_M2
# define __GLIBC_RISCV_MATHVEC_EXPF_M2 0
#endif
#if __GLIBC_RISCV_MATHVEC_EXPF_M2 && defined __FAST_MATH__ \
    && ((defined __riscv_simd_math_expf_m2 \
         && __riscv_simd_math_expf_m2 == 1) \
        || (defined __riscv_simd_math_expf_m2_vla \
            && __riscv_simd_math_expf_m2_vla == 1))
# undef __DECL_SIMD_expf
# define __DECL_SIMD_expf __attribute__ ((__simd__ ("notinbranch"), const))
#endif

#endif /* New compiler/library handshake.  */
