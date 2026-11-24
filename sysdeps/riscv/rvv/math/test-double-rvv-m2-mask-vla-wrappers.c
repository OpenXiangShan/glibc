/* Candidate all-active scalable masked generic-test adapter.
   Copyright (C) 2026 Free Software Foundation, Inc.
   This file is part of the GNU C Library.  The GNU C Library is free software;
   you can redistribute it and/or modify it under the GNU Lesser General
   Public License, version 2.1 or (at your option) any later version.  */
#include <riscv_vector.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fenv.h>
extern vfloat64m2_t _ZGV2Mxv_log (vbool32_t, vfloat64m2_t);
unsigned long rvv_math_test_vlmax (void) { return __riscv_vsetvlmax_e64m2 (); }
double log_rvv_m2_mask_vla (double x)
{
  size_t vl = __riscv_vsetvlmax_e64m2 ();
  vfloat64m2_t vx = __riscv_vfmv_v_f_f64m2 (x, vl);
  vuint64m2_t ones = __riscv_vmv_v_x_u64m2 (1, vl);
  vbool32_t mask = __riscv_vmsne_vx_u64m2_b32 (ones, 0, vl);
  vfloat64m2_t y = _ZGV2Mxv_log (mask, vx);
  double values[vl]; uint64_t first;
  __riscv_vse64_v_f64m2 (values, y, vl);
  memcpy (&first, values, sizeof first);
  for (size_t i = 1; i < vl; ++i) {
    uint64_t bits; memcpy (&bits, values + i, sizeof bits);
    if (bits != first) { fprintf (stderr, "mask Mx lane %zu mismatch\n", i); exit (1); }
  }
  return values[0];
}

extern vfloat64m2_t _ZGV2Mxv_exp (vbool32_t, vfloat64m2_t);
static void
check_exp_inactive_m2_vla (void)
{
  size_t vl = __riscv_vsetvlmax_e64m2 ();
  uint64_t bits[vl];
  for (size_t i = 0; i < vl; ++i)
    bits[i] = UINT64_C (0x7ff0000000000001);
  vfloat64m2_t vx = __riscv_vreinterpret_v_u64m2_f64m2 (__riscv_vle64_v_u64m2 (bits, vl));
  vuint64m2_t zero = __riscv_vmv_v_x_u64m2 (0, vl);
  vbool32_t mask = __riscv_vmsne_vv_u64m2_b32 (zero, zero, vl);
  fenv_t env;
  fegetenv (&env);
  feclearexcept (FE_ALL_EXCEPT);
  vfloat64m2_t y = _ZGV2Mxv_exp (mask, vx);
  int flags = fetestexcept (FE_ALL_EXCEPT);
  fesetenv (&env);
  (void) y;
  if (flags)
    {
      fprintf (stderr, "m2 mask Mx inactive flags %#x\n", flags);
      exit (1);
    }
}

double exp_rvv_m2_mask_vla (double x)
{
  check_exp_inactive_m2_vla ();
  size_t vl = __riscv_vsetvlmax_e64m2 ();
  vfloat64m2_t vx = __riscv_vfmv_v_f_f64m2 (x, vl);
  vuint64m2_t ones = __riscv_vmv_v_x_u64m2 (1, vl);
  vbool32_t mask = __riscv_vmsne_vx_u64m2_b32 (ones, 0, vl);
  vfloat64m2_t y = _ZGV2Mxv_exp (mask, vx);
  double values[vl]; uint64_t first;
  __riscv_vse64_v_f64m2 (values, y, vl);
  memcpy (&first, values, sizeof first);
  for (size_t i = 1; i < vl; ++i) {
    uint64_t bits; memcpy (&bits, values + i, sizeof bits);
    if (bits != first) { fprintf (stderr, "mask Mx lane %zu mismatch\n", i); exit (1); }
  }
  return values[0];
}

extern vfloat64m2_t _ZGV2Mxv_sin (vbool32_t, vfloat64m2_t);
double sin_rvv_m2_mask_vla (double x)
{
  size_t vl = __riscv_vsetvlmax_e64m2 ();
  vfloat64m2_t vx = __riscv_vfmv_v_f_f64m2 (x, vl);
  vuint64m2_t ones = __riscv_vmv_v_x_u64m2 (1, vl);
  vbool32_t mask = __riscv_vmsne_vx_u64m2_b32 (ones, 0, vl);
  vfloat64m2_t y = _ZGV2Mxv_sin (mask, vx);
  double values[vl]; uint64_t bits[vl];
  __riscv_vse64_v_f64m2 (values, y, vl);
  memcpy (bits, values, sizeof bits);
  for (size_t i = 1; i < vl; ++i)
    if (bits[i] != bits[0])
      { fprintf (stderr, "sin vla lane %zu mismatch\n", i); exit (1); }
  return values[0];
}

extern vfloat64m2_t _ZGV2Mxv_cos (vbool32_t, vfloat64m2_t);
double cos_rvv_m2_mask_vla (double x)
{
  size_t vl = __riscv_vsetvlmax_e64m2 ();
  vfloat64m2_t vx = __riscv_vfmv_v_f_f64m2 (x, vl);
  vuint64m2_t ones = __riscv_vmv_v_x_u64m2 (1, vl);
  vbool32_t mask = __riscv_vmsne_vx_u64m2_b32 (ones, 0, vl);
  vfloat64m2_t y = _ZGV2Mxv_cos (mask, vx);
  double values[vl]; uint64_t bits[vl];
  __riscv_vse64_v_f64m2 (values, y, vl);
  memcpy (bits, values, sizeof bits);
  for (size_t i = 1; i < vl; ++i)
    if (bits[i] != bits[0])
      { fprintf (stderr, "cos vla lane %zu mismatch\n", i); exit (1); }
  return values[0];
}

extern vfloat64m2_t _ZGV2Mxvv_pow (vbool32_t, vfloat64m2_t, vfloat64m2_t);
double pow_rvv_m2_mask_vla (double x, double y)
{
  size_t vl = __riscv_vsetvlmax_e64m2 ();
  vfloat64m2_t vx = __riscv_vfmv_v_f_f64m2 (x, vl);
  vfloat64m2_t vy = __riscv_vfmv_v_f_f64m2 (y, vl);
  vuint64m2_t ones = __riscv_vmv_v_x_u64m2 (1, vl);
  vbool32_t mask = __riscv_vmsne_vx_u64m2_b32 (ones, 0, vl);
  vfloat64m2_t z = _ZGV2Mxvv_pow (mask, vx, vy);
  double values[vl]; uint64_t bits[vl];
  __riscv_vse64_v_f64m2 (values, z, vl);
  memcpy (bits, values, sizeof bits);
  for (size_t i = 1; i < vl; ++i)
    if (bits[i] != bits[0])
      { fprintf (stderr, "pow vla lane %zu mismatch\n", i); exit (1); }
  return values[0];
}