/* Runtime RVV capability check for generic vector math tests.
   Copyright (C) 2026 Free Software Foundation, Inc.
   This file is part of the GNU C Library.

   The GNU C Library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public License as
   published by the Free Software Foundation; either version 2.1 of the
   License, or (at your option) any later version.  */
#if defined REQUIRE_RVV_M2 || defined REQUIRE_RVV_M1 \
    || defined REQUIRE_RVV_M4 || defined REQUIRE_RVV_M8
# include <stdio.h>
# include <stdlib.h>
# include <sys/auxv.h>
# include <support/test-driver.h>
extern unsigned long rvv_math_test_vlmax (void);
/* The minimum VLMAX is the ABI lane count of the widest test entry at the
   128-bit baseline.  A float test reports twice the double lane count from
   the same LMUL, so the double threshold is a valid lower bound for both
   precisions; VLEN below 128 is rejected by the interface contract anyway.  */
# if defined REQUIRE_RVV_M8
#  define RVV_MATH_TEST_MIN_VLMAX 16
# elif defined REQUIRE_RVV_M4
#  define RVV_MATH_TEST_MIN_VLMAX 8
# elif defined REQUIRE_RVV_M1
#  define RVV_MATH_TEST_MIN_VLMAX 2
# else
#  define RVV_MATH_TEST_MIN_VLMAX 4
# endif
# define INIT_ARCH_EXT do { \
  if (!(getauxval (AT_HWCAP) & (1UL << ('V' - 'A')))) \
    { puts ("RVV unavailable"); exit (EXIT_UNSUPPORTED); } \
  if (rvv_math_test_vlmax () < RVV_MATH_TEST_MIN_VLMAX) \
    { puts ("RVV VLEN below 128 bits"); exit (EXIT_UNSUPPORTED); } \
} while (0)
# define CHECK_ARCH_EXT
#else
# include <sysdeps/generic/math-tests-arch.h>
#endif
