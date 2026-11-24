/* Check when variant calling-convention PLT symbols are resolved.
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

static int variant_resolved;
static int ordinary_resolved;

static int
variant_impl (int x)
{
  return x + 1;
}

static int
ordinary_impl (int x)
{
  return x + 2;
}

static __typeof (variant_impl) *
resolve_variant (void)
{
  variant_resolved = 1;
  return variant_impl;
}

static __typeof (ordinary_impl) *
resolve_ordinary (void)
{
  ordinary_resolved = 1;
  return ordinary_impl;
}

int variant_cc_call (int) __attribute__ ((ifunc ("resolve_variant")));
int ordinary_cc_call (int) __attribute__ ((ifunc ("resolve_ordinary")));

/* No vector instructions are needed to test the ELF calling-convention
   marker.  This also lets the test run on machines without V.  */
__asm__ (".variant_cc variant_cc_call");

int
variant_cc_state (void)
{
  return variant_resolved | (ordinary_resolved << 1);
}
