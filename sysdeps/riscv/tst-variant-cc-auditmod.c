/* Check audit symbol binding for eagerly resolved variant CC symbols.
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

#include <link.h>
#include <stdint.h>
#include <string.h>

unsigned int
la_version (unsigned int version)
{
  return LAV_CURRENT;
}

unsigned int
la_objopen (struct link_map *map, Lmid_t lmid, uintptr_t *cookie)
{
  return LA_FLG_BINDTO | LA_FLG_BINDFROM;
}

/* This test symbol is scalar but carries the variant CC marker, so the
   audit replacement does not need any vector instructions.  */
static int
audit_variant (int x)
{
  return x + 5;
}

#if __ELF_NATIVE_CLASS == 64
# define symbind la_symbind64
#else
# define symbind la_symbind32
#endif

uintptr_t
symbind (ElfW(Sym) *sym, unsigned int ndx, uintptr_t *refcook,
         uintptr_t *defcook, unsigned int *flags, const char *symname)
{
  if (strcmp (symname, "variant_cc_call") == 0)
    return (uintptr_t) audit_variant;
  return sym->st_value;
}
