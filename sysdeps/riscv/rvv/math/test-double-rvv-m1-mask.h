/* Copyright (C) 2026 Free Software Foundation, Inc.
   This file is part of the GNU C Library.

   The GNU C Library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public License as
   published by the Free Software Foundation; either version 2.1 of the
   License, or (at your option) any later version.  */
/* Candidate generic all-active LMUL1 masked vector log tests.  */
#include "test-double.h"
#include "test-math-vector.h"
#define REQUIRE_RVV_M1
#define VEC_SUFF _rvv_m1_mask
#define VEC_LEN 2
