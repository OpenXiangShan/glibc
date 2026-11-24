/* RVV-only adapter for generic scalar log test inputs.
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

extern vfloat64m2_t _ZGV2N4v_log (vfloat64m2_t);
extern vfloat64m2_t _ZGV2N4v_exp (vfloat64m2_t);

unsigned long
rvv_math_test_vlmax (void)
{
  return __riscv_vsetvlmax_e64m2 ();
}

double
log_rvv_m2 (double x)
{
  vfloat64m2_t vx = __riscv_vfmv_v_f_f64m2 (x, 4);
  vfloat64m2_t vy = _ZGV2N4v_log (vx);
  double values[4];
  __riscv_vse64_v_f64m2 (values, vy, 4);
  /* A broadcast must produce identical lanes, including NaN bit patterns.  */
  uint64_t bits[4];
  memcpy (bits, values, sizeof bits);
  for (int i = 1; i < 4; ++i)
    if (bits[0] != bits[i])
      {
        fprintf (stderr, "RVV log broadcast mismatch in lane %d\n", i);
        exit (1);
      }
  return values[0];
}

double
exp_rvv_m2 (double x)
{
  vfloat64m2_t vx = __riscv_vfmv_v_f_f64m2 (x, 4);
  vfloat64m2_t vy = _ZGV2N4v_exp (vx);
  double values[4];
  uint64_t bits[4];
  __riscv_vse64_v_f64m2 (values, vy, 4);
  memcpy (bits, values, sizeof bits);
  for (int i = 1; i < 4; ++i)
    if (bits[0] != bits[i])
      {
        fprintf (stderr, "RVV exp broadcast mismatch in lane %d\n", i);
        exit (1);
      }
  return values[0];
}

static double
rvv_extension_result (vfloat64m2_t vy, size_t vl)
{
  double values[vl];
  uint64_t first;
  __riscv_vse64_v_f64m2 (values, vy, vl);
  memcpy (&first, values, sizeof first);
  for (size_t i = 1; i < vl; ++i)
    {
      uint64_t bits;
      memcpy (&bits, &values[i], sizeof bits);
      if (first != bits)
        { fprintf (stderr, "RVV extension lane %zu mismatch\n", i); exit (1); }
    }
  return values[0];
}

extern vfloat64m2_t _ZGV2N4v_sin (vfloat64m2_t);
double
sin_rvv_m2 (double x)
{
  size_t vl = 4;
  return rvv_extension_result (_ZGV2N4v_sin (__riscv_vfmv_v_f_f64m2 (x, vl)), vl);
}

extern vfloat64m2_t _ZGV2N4v_cos (vfloat64m2_t);
double
cos_rvv_m2 (double x)
{
  size_t vl = 4;
  return rvv_extension_result (_ZGV2N4v_cos (__riscv_vfmv_v_f_f64m2 (x, vl)), vl);
}

extern vfloat64m2_t _ZGV2N4vv_pow (vfloat64m2_t, vfloat64m2_t);
double
pow_rvv_m2 (double x, double y)
{
  size_t vl = 4;
  return rvv_extension_result (_ZGV2N4vv_pow (__riscv_vfmv_v_f_f64m2 (x, vl), __riscv_vfmv_v_f_f64m2 (y, vl)), vl);
}
