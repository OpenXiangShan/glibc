/* RVV-only adapter for generic LMUL1 scalar log/exp/sin/cos/pow test inputs.
   Copyright (C) 2026 Free Software Foundation, Inc.
   This file is part of the GNU C Library.

   The GNU C Library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public License as
   published by the Free Software Foundation; either version 2.1 of the
   License, or (at your option) any later version.  */
#include <riscv_vector.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

extern vfloat64m1_t _ZGV1N2v_log (vfloat64m1_t);
extern vfloat64m1_t _ZGV1N2v_exp (vfloat64m1_t);

unsigned long
rvv_math_test_vlmax (void)
{
  return __riscv_vsetvlmax_e64m1 ();
}

static double
rvv_m1_result (vfloat64m1_t vy, size_t vl)
{
  double values[vl];
  uint64_t first;
  __riscv_vse64_v_f64m1 (values, vy, vl);
  memcpy (&first, values, sizeof first);
  for (size_t i = 1; i < vl; ++i)
    {
      uint64_t bits;
      memcpy (&bits, &values[i], sizeof bits);
      if (first != bits)
        { fprintf (stderr, "RVV m1 lane %zu mismatch\n", i); exit (1); }
    }
  return values[0];
}

double
log_rvv_m1 (double x)
{
  size_t vl = 2;
  return rvv_m1_result (_ZGV1N2v_log (__riscv_vfmv_v_f_f64m1 (x, vl)), vl);
}

double
exp_rvv_m1 (double x)
{
  size_t vl = 2;
  return rvv_m1_result (_ZGV1N2v_exp (__riscv_vfmv_v_f_f64m1 (x, vl)), vl);
}

extern vfloat64m1_t _ZGV1N2v_sin (vfloat64m1_t);
double
sin_rvv_m1 (double x)
{
  size_t vl = 2;
  return rvv_m1_result (_ZGV1N2v_sin (__riscv_vfmv_v_f_f64m1 (x, vl)), vl);
}

extern vfloat64m1_t _ZGV1N2v_cos (vfloat64m1_t);
double
cos_rvv_m1 (double x)
{
  size_t vl = 2;
  return rvv_m1_result (_ZGV1N2v_cos (__riscv_vfmv_v_f_f64m1 (x, vl)), vl);
}

extern vfloat64m1_t _ZGV1N2vv_pow (vfloat64m1_t, vfloat64m1_t);
double
pow_rvv_m1 (double x, double y)
{
  size_t vl = 2;
  return rvv_m1_result (_ZGV1N2vv_pow (__riscv_vfmv_v_f_f64m1 (x, vl),
                                       __riscv_vfmv_v_f_f64m1 (y, vl)), vl);
}
