/* Exercise variant CC PLT relocations in a dlopened object.
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

extern int variant_cc_call (int);
extern int ordinary_cc_call (int);
extern int variant_cc_state (void);
__asm__ (".variant_cc variant_cc_call");

int
variant_cc_dlopen_check (int expected)
{
  int before = variant_cc_state ();
  int v = variant_cc_call (41);
  int middle = variant_cc_state ();
  int s = ordinary_cc_call (40);
  int after = variant_cc_state ();
  return before != expected || middle != expected || after != 3
         || v != 42 || s != 42;
}
