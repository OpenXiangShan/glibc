/* RVV-only adapter for generic scalable log tests.
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

extern vfloat64m2_t _ZGV2Nxv_log (vfloat64m2_t);
extern vfloat64m2_t _ZGV2Nxv_exp (vfloat64m2_t);

unsigned long
rvv_math_test_vlmax (void)
{
  return __riscv_vsetvlmax_e64m2 ();
}

double
log_rvv_m2_vla (double x)
{
  size_t vl = __riscv_vsetvlmax_e64m2 ();
  double values[vl];
  vfloat64m2_t vx = __riscv_vfmv_v_f_f64m2 (x, vl);
  vfloat64m2_t vy = _ZGV2Nxv_log (vx);
  __riscv_vse64_v_f64m2 (values, vy, vl);
  uint64_t first;
  memcpy (&first, &values[0], sizeof first);
  for (size_t i = 1; i < vl; ++i)
    {
      uint64_t bits;
      memcpy (&bits, &values[i], sizeof bits);
      if (bits != first)
        {
          fprintf (stderr, "RVV VLA log broadcast mismatch in lane %zu\n", i);
          exit (1);
        }
    }
  return values[0];
}

double
exp_rvv_m2_vla (double x)
{
  size_t vl = __riscv_vsetvlmax_e64m2 ();
  double values[vl];
  vfloat64m2_t vx = __riscv_vfmv_v_f_f64m2 (x, vl);
  vfloat64m2_t vy = _ZGV2Nxv_exp (vx);
  __riscv_vse64_v_f64m2 (values, vy, vl);
  uint64_t first;
  memcpy (&first, &values[0], sizeof first);
  for (size_t i = 1; i < vl; ++i)
    {
      uint64_t bits;
      memcpy (&bits, &values[i], sizeof bits);
      if (bits != first)
        {
          fprintf (stderr, "RVV VLA exp broadcast mismatch in lane %zu\n", i);
          exit (1);
        }
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

extern vfloat64m2_t _ZGV2Nxv_sin (vfloat64m2_t);
double
sin_rvv_m2_vla (double x)
{
  size_t vl = __riscv_vsetvlmax_e64m2 ();
  return rvv_extension_result (_ZGV2Nxv_sin (__riscv_vfmv_v_f_f64m2 (x, vl)), vl);
}

extern vfloat64m2_t _ZGV2Nxv_cos (vfloat64m2_t);
double
cos_rvv_m2_vla (double x)
{
  size_t vl = __riscv_vsetvlmax_e64m2 ();
  return rvv_extension_result (_ZGV2Nxv_cos (__riscv_vfmv_v_f_f64m2 (x, vl)), vl);
}

extern vfloat64m2_t _ZGV2Nxvv_pow (vfloat64m2_t, vfloat64m2_t);
double
pow_rvv_m2_vla (double x, double y)
{
  size_t vl = __riscv_vsetvlmax_e64m2 ();
  return rvv_extension_result (_ZGV2Nxvv_pow (__riscv_vfmv_v_f_f64m2 (x, vl), __riscv_vfmv_v_f_f64m2 (y, vl)), vl);
}
