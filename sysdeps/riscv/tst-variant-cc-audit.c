/* Check that variant CC binding honors la_symbind redirection.
   Copyright (C) 2026 Free Software Foundation, Inc.
   This file is part of the GNU C Library.

   The GNU C Library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public License as
   published by the Free Software Foundation; either version 2.1 of the
   License, or (at your option) any later version.

   The GNU C Library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with the GNU C Library; if not, see
   <https://www.gnu.org/licenses/>.  */

#include <support/check.h>

extern int variant_cc_call (int);
extern int ordinary_cc_call (int);
__asm__ (".variant_cc variant_cc_call");

static int
do_test (void)
{
  TEST_COMPARE (variant_cc_call (37), 42);
  TEST_COMPARE (ordinary_cc_call (40), 42);
  return 0;
}

#include <support/test-driver.c>
