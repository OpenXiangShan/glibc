/* RVV-only test adapter; the test driver itself is baseline RV64GC.
   Copyright (C) 2026 Free Software Foundation, Inc.
   This file is part of the GNU C Library.

   The GNU C Library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public License as
   published by the Free Software Foundation; either version 2.1 of the
   License, or (at your option) any later version.  */
#include <riscv_vector.h>
#include <stddef.h>

extern vfloat64m2_t _ZGV2N4v_log (vfloat64m2_t);

size_t
rvv_log_test_vlmax (void)
{
  return __riscv_vsetvlmax_e64m2 ();
}

void
rvv_log_test_call (const double *input, double *output)
{
  vfloat64m2_t x = __riscv_vle64_v_f64m2 (input, 4);
  vfloat64m2_t y = _ZGV2N4v_log (x);
  __riscv_vse64_v_f64m2 (output, y, 4);
}
