/* Mixed-lane and floating environment tests for the RVV log ABI.
   Copyright (C) 2026 Free Software Foundation, Inc.
   This file is part of the GNU C Library.

   The GNU C Library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public License as
   published by the Free Software Foundation; either version 2.1 of the
   License, or (at your option) any later version.

   The finite expected values were rounded from MPFR 4.2.1 at 256 bits.  */
#include <fenv.h>
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <sys/auxv.h>
#include <support/check.h>

extern size_t rvv_log_test_vlmax (void);
extern void rvv_log_test_call (const double *, double *);

static uint64_t
ordered (double x)
{
  uint64_t u;
  memcpy (&u, &x, sizeof u);
  return (u >> 63) ? ~u : u | (UINT64_C (1) << 63);
}

static int
do_test (void)
{
  /* Linux RISC-V AT_HWCAP assigns bit ('V' - 'A') to V.  */
  if (!(getauxval (AT_HWCAP) & (1UL << ('V' - 'A'))))
    FAIL_UNSUPPORTED ("RVV is not available");
  if (rvv_log_test_vlmax () < 4)
    FAIL_UNSUPPORTED ("RVV VLEN is below 128 bits");

  static const double values[12] = {
    0x0.0000000000001p-1022, 0x0.0000000000002p-1022,
    0x0.fffffffffffffp-1022, 0x1p-1022,
    0x1.0000000000001p-1022, 0x1.fffffffffffffp-1,
    1.0, 0x1.0000000000001p+0,
    0.5, 1.5, 2.0, 0x1.fffffffffffffp+1023
  };
  static const double expected[12] = {
    -0x1.74385446d71c3p+9, -0x1.73df9b3adb335p+9,
    -0x1.6232bdd7abcd2p+9, -0x1.6232bdd7abcd2p+9,
    -0x1.6232bdd7abcd2p+9, -0x1p-53,
    0.0, 0x1.fffffffffffffp-53,
    -0x1.62e42fefa39efp-1, 0x1.9f323ecbf984cp-2,
    0x1.62e42fefa39efp-1, 0x1.62e42fefa39efp+9
  };
  double out[4];
  for (int i = 0; i < 12; i += 4)
    {
      rvv_log_test_call (values + i, out);
      for (int j = 0; j < 4; ++j)
        {
          uint64_t a = ordered (out[j]), b = ordered (expected[i+j]);
          uint64_t diff = a > b ? a-b : b-a;
          if (diff > 2)
            FAIL_EXIT1 ("log(%a) = %a, expected %a (ULP %llu)",
                        values[i+j], out[j], expected[i+j],
                        (unsigned long long) diff);
        }
    }
  const double mixed[4] = { 1.0, -1.0, 0.0, INFINITY };
  feclearexcept (FE_ALL_EXCEPT);
  rvv_log_test_call (mixed, out);
  TEST_VERIFY (out[0] == 0.0 && !signbit (out[0]));
  TEST_VERIFY (isnan (out[1]));
  TEST_VERIFY (isinf (out[2]) && signbit (out[2]));
  TEST_VERIFY (isinf (out[3]) && !signbit (out[3]));
  TEST_VERIFY ((fetestexcept (FE_INVALID | FE_DIVBYZERO)
                & (FE_INVALID | FE_DIVBYZERO)) == (FE_INVALID | FE_DIVBYZERO));

  const double special[4] = { -0.0, -INFINITY, NAN, __builtin_nans ("") };
  feclearexcept (FE_ALL_EXCEPT);
  rvv_log_test_call (special, out);
  TEST_VERIFY (isinf (out[0]) && signbit (out[0]));
  TEST_VERIFY (isnan (out[1]) && isnan (out[2]) && isnan (out[3]));
  TEST_VERIFY ((fetestexcept (FE_INVALID | FE_DIVBYZERO)
                & (FE_INVALID | FE_DIVBYZERO)) == (FE_INVALID | FE_DIVBYZERO));

  int saved = fegetround ();
  int modes[] = { FE_TONEAREST, FE_DOWNWARD, FE_UPWARD, FE_TOWARDZERO };
  for (unsigned i = 0; i < sizeof modes / sizeof modes[0]; ++i)
    {
      TEST_COMPARE (fesetround (modes[i]), 0);
      feclearexcept (FE_ALL_EXCEPT);
      feraiseexcept (FE_OVERFLOW);
      rvv_log_test_call (values + 8, out);
      TEST_COMPARE (fegetround (), modes[i]);
      TEST_VERIFY (fetestexcept (FE_OVERFLOW));
    }
  fesetround (saved);
  return 0;
}
#include <support/test-driver.c>
