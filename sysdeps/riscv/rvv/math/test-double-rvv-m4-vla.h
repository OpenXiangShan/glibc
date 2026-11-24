/* Scalable LMUL4 double-vector math tests.
   Copyright (C) 2026 Free Software Foundation, Inc.
   This file is part of the GNU C Library.
   The GNU C Library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public License as
   published by the Free Software Foundation; either version 2.1 of the
   License, or (at your option) any later version.  */
#include "test-double.h"
#include "test-math-vector.h"
#define REQUIRE_RVV_M4
#define VEC_SUFF _rvv_m4_vla
#define VEC_LEN VLMAX
