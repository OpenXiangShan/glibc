/* Test selective eager binding with dlopen.
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

#include <stdlib.h>
#include <support/check.h>
#include <support/xdlfcn.h>

static int
do_test (void)
{
  const char *bind_now = getenv ("LD_BIND_NOW");
  int lazy_expected = bind_now != NULL && bind_now[0] != '\0' ? 3 : 1;
  const int modes[] = { RTLD_LAZY, RTLD_NOW };
  for (unsigned int i = 0; i < sizeof modes / sizeof modes[0]; ++i)
    {
      void *handle = xdlopen ("tst-variant-cc-dlopenmod.so",
                             modes[i] | RTLD_LOCAL);
      int (*check) (int) = xdlsym (handle, "variant_cc_dlopen_check");
      TEST_COMPARE (check (i == 0 ? lazy_expected : 3), 0);
      xdlclose (handle);
    }
  return 0;
}

#include <support/test-driver.c>
