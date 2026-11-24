/* RVV-only adapter for generic double tests, LMUL8 fixed.
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

extern vfloat64m8_t _ZGV8N16v_log (vfloat64m8_t);
extern vfloat64m8_t _ZGV8N16v_exp (vfloat64m8_t);
extern vfloat64m8_t _ZGV8N16v_sin (vfloat64m8_t);
extern vfloat64m8_t _ZGV8N16v_cos (vfloat64m8_t);
extern vfloat64m8_t _ZGV8N16vv_pow (vfloat64m8_t, vfloat64m8_t);

unsigned long
rvv_math_test_vlmax (void)
{
  return __riscv_vsetvlmax_e64m8 ();
}

static double
rvv_lmul_identity (vfloat64m8_t vy, size_t vl)
{
  double values[vl];
  uint64_t first;
  __riscv_vse64_v_f64m8 (values, vy, vl);
  memcpy (&first, values, sizeof first);
  for (size_t i = 1; i < vl; ++i)
    {
      uint64_t lane;
      memcpy (&lane, &values[i], sizeof lane);
      if (lane != first)
        { fprintf (stderr, "RVV LMUL8 lane %zu mismatch\n", i); exit (1); }
    }
  return values[0];
}

double
log_rvv_m8 (double x)
{
  size_t vl = 16;
  vfloat64m8_t vx = __riscv_vfmv_v_f_f64m8 (x, vl);
  return rvv_lmul_identity (_ZGV8N16v_log (vx), vl);
}

double
exp_rvv_m8 (double x)
{
  size_t vl = 16;
  vfloat64m8_t vx = __riscv_vfmv_v_f_f64m8 (x, vl);
  return rvv_lmul_identity (_ZGV8N16v_exp (vx), vl);
}

double
sin_rvv_m8 (double x)
{
  size_t vl = 16;
  vfloat64m8_t vx = __riscv_vfmv_v_f_f64m8 (x, vl);
  return rvv_lmul_identity (_ZGV8N16v_sin (vx), vl);
}

double
cos_rvv_m8 (double x)
{
  size_t vl = 16;
  vfloat64m8_t vx = __riscv_vfmv_v_f_f64m8 (x, vl);
  return rvv_lmul_identity (_ZGV8N16v_cos (vx), vl);
}

double
pow_rvv_m8 (double x, double y)
{
  size_t vl = 16;
  vfloat64m8_t vx = __riscv_vfmv_v_f_f64m8 (x, vl);
  vfloat64m8_t vy = __riscv_vfmv_v_f_f64m8 (y, vl);
  return rvv_lmul_identity (_ZGV8N16vv_pow (vx, vy), vl);
}
