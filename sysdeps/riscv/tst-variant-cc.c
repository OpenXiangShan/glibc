/* Test eager binding of variant CC symbols in an otherwise lazy object.
   Copyright (C) 2026 Free Software Foundation, Inc.
   This file is part of the GNU C Library.

   The GNU C Library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   The GNU C Library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with the GNU C Library; if not, see
   <https://www.gnu.org/licenses/>.  */

#include <stdlib.h>
#include <support/check.h>

extern int variant_cc_call (int);
extern int ordinary_cc_call (int);
extern int variant_cc_state (void);
__asm__ (".variant_cc variant_cc_call");

static int
do_test (void)
{
  const char *bind_now = getenv ("LD_BIND_NOW");
  int expected = bind_now != NULL && bind_now[0] != '\0' ? 3 : 1;

  /* Inspect resolver side effects before making either call.  The ordinary
     symbol must still be lazy, unless LD_BIND_NOW was explicitly requested.  */
  TEST_COMPARE (variant_cc_state (), expected);
  TEST_COMPARE (variant_cc_call (41), 42);
  TEST_COMPARE (variant_cc_state (), expected);
  TEST_COMPARE (ordinary_cc_call (40), 42);
  TEST_COMPARE (variant_cc_state (), 3);
  return 0;
}

#include <support/test-driver.c>
