/* RVV binary64 types and helpers for the mathvec kernels.
   Copyright (C) 2026 Free Software Foundation, Inc.
   This file is part of the GNU C Library.
   SPDX-License-Identifier: LGPL-2.1-or-later */

#include <math.h>
#include <stddef.h>
#include <stdint.h>
#include <riscv_vector.h>

#define RVV_CAT_(a, b) a##b
#define RVV_CAT(a, b) RVV_CAT_ (a, b)
#define FT RVV_CAT (f64m, RVV_LMUL)
#define UT RVV_CAT (u64m, RVV_LMUL)
#define IT RVV_CAT (i64m, RVV_LMUL)
#define BT RVV_CAT (b, RVV_MASK_BITS)
#define FV RVV_CAT (RVV_CAT (vfloat64m, RVV_LMUL), _t)
#define UV RVV_CAT (RVV_CAT (vuint64m, RVV_LMUL), _t)
#define IV RVV_CAT (RVV_CAT (vint64m, RVV_LMUL), _t)
#define BV RVV_CAT (RVV_CAT (vbool, RVV_MASK_BITS), _t)
#define RVV_OP_(a, t) __riscv_##a##_##t
#define RVV_OP(a, t) RVV_OP_ (a, t)

#define REINTERPRET_FU RVV_CAT (RVV_CAT (__riscv_vreinterpret_v_, FT), RVV_CAT (_, UT))
#define REINTERPRET_UF RVV_CAT (RVV_CAT (__riscv_vreinterpret_v_, UT), RVV_CAT (_, FT))
#define REINTERPRET_UI RVV_CAT (RVV_CAT (__riscv_vreinterpret_v_, UT), RVV_CAT (_, IT))
#define UCMPLT RVV_CAT (RVV_OP (vmsltu_vx, UT), RVV_CAT (_, BT))
#define UCMPGT RVV_CAT (RVV_OP (vmsgtu_vx, UT), RVV_CAT (_, BT))
#define UCMPNE RVV_CAT (RVV_OP (vmsne_vx, UT), RVV_CAT (_, BT))
#define FCMPLT RVV_CAT (RVV_OP (vmflt_vf, FT), RVV_CAT (_, BT))
#define UGE RVV_CAT (RVV_OP (vmsgeu_vx, UT), RVV_CAT (_, BT))
#define CONVERT RVV_OP (vfcvt_f_x_v, FT)
#define UGATHER RVV_OP (vluxei64_v, UT)
#define SETVL RVV_OP (vsetvl, RVV_CAT (e64m, RVV_LMUL))
#define SETVLMAX RVV_OP (vsetvlmax, RVV_CAT (e64m, RVV_LMUL))

#if RVV_LMUL <= 2
/* Use integer operations before the kernel so masked-off sNaNs, domain
   errors and scalar fallbacks cannot contribute errno or exceptions. */
static inline FV
rvv_mask_input (BV mask, FV x, uint64_t neutral, size_t vl)
{
  UV fill = RVV_OP (vmv_v_x, UT) (neutral, vl);
  UV safe = RVV_OP (vmerge_vvm, UT) (fill, REINTERPRET_FU (x), mask, vl);
  return REINTERPRET_UF (safe);
}
#endif
