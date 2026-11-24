/* RVV-only adapter for generic double tests, LMUL4 vla.
   Copyright (C) 2026 Free Software Foundation, Inc.
   This file is part of the GNU C Library.

   The GNU C Library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public License as
   published by the Free Software Foundation; either version 2.1 of the
   License, or (at your option) any later version.  */
#include <riscv_vector.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern vfloat64m4_t _ZGV4Nxv_log (vfloat64m4_t);
extern vfloat64m4_t _ZGV4Nxv_exp (vfloat64m4_t);
extern vfloat64m4_t _ZGV4Nxv_sin (vfloat64m4_t);
extern vfloat64m4_t _ZGV4Nxv_cos (vfloat64m4_t);
extern vfloat64m4_t _ZGV4Nxvv_pow (vfloat64m4_t, vfloat64m4_t);

unsigned long
rvv_math_test_vlmax (void)
{
  return __riscv_vsetvlmax_e64m4 ();
}

static double
rvv_lmul_identity (vfloat64m4_t vy, size_t vl)
{
  double values[vl];
  uint64_t first;
  __riscv_vse64_v_f64m4 (values, vy, vl);
  memcpy (&first, values, sizeof first);
  for (size_t i = 1; i < vl; ++i)
    {
      uint64_t lane;
      memcpy (&lane, &values[i], sizeof lane);
      if (lane != first)
        { fprintf (stderr, "RVV LMUL4 lane %zu mismatch\n", i); exit (1); }
    }
  return values[0];
}

double
log_rvv_m4_vla (double x)
{
  size_t vl = __riscv_vsetvlmax_e64m4 ();
  vfloat64m4_t vx = __riscv_vfmv_v_f_f64m4 (x, vl);
  return rvv_lmul_identity (_ZGV4Nxv_log (vx), vl);
}

double
exp_rvv_m4_vla (double x)
{
  size_t vl = __riscv_vsetvlmax_e64m4 ();
  vfloat64m4_t vx = __riscv_vfmv_v_f_f64m4 (x, vl);
  return rvv_lmul_identity (_ZGV4Nxv_exp (vx), vl);
}

double
sin_rvv_m4_vla (double x)
{
  size_t vl = __riscv_vsetvlmax_e64m4 ();
  vfloat64m4_t vx = __riscv_vfmv_v_f_f64m4 (x, vl);
  return rvv_lmul_identity (_ZGV4Nxv_sin (vx), vl);
}

double
cos_rvv_m4_vla (double x)
{
  size_t vl = __riscv_vsetvlmax_e64m4 ();
  vfloat64m4_t vx = __riscv_vfmv_v_f_f64m4 (x, vl);
  return rvv_lmul_identity (_ZGV4Nxv_cos (vx), vl);
}

double
pow_rvv_m4_vla (double x, double y)
{
  size_t vl = __riscv_vsetvlmax_e64m4 ();
  vfloat64m4_t vx = __riscv_vfmv_v_f_f64m4 (x, vl);
  vfloat64m4_t vy = __riscv_vfmv_v_f_f64m4 (y, vl);
  return rvv_lmul_identity (_ZGV4Nxvv_pow (vx, vy), vl);
}
