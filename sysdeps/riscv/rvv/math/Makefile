# The downstream RISC-V mathvec implementation is built for RVV separately
# from the baseline rv64gc libc.  This directory is selected only for lp64d.
include $(..)sysdeps/riscv/rvv/math/mathvec-variants.mk

ifeq ($(subdir),mathvec)
# The constant tables are separate objects so a routine only pulls in the
# tables of its own kernel family.
libmvec-support += exp_data log_data pow_data sincos_data
libmvec-support += $(sort $(foreach v,$(rvv-mathvec-variants),$(rvv-mathvec-$(v)-routine)))
$(foreach v,$(rvv-mathvec-variants),$(eval CFLAGS-$(rvv-mathvec-$(v)-routine).c += -march=rv64gcv -mabi=lp64d))
# Keep the native m1 kernels on the m1 code-generation strategy.  Filter the
# manifest by LMUL so an m2 routine never gets the scoped flag.
rvv-mathvec-m1-routines := $(sort $(foreach v,$(rvv-mathvec-variants),\
  $(if $(filter 1,$(rvv-mathvec-$(v)-lmul)),$(rvv-mathvec-$(v)-routine))))
$(foreach r,$(rvv-mathvec-m1-routines),$(eval CFLAGS-$(r).c += -mrvv-max-lmul=m1))
endif

ifeq ($(subdir),math)
# Keep installed declarations consistent with --disable-mathvec.
headers += bits/riscv-mathvec.h
generated += bits/riscv-mathvec.h
before-compile += $(objpfx)bits/riscv-mathvec.h
$(objpfx)bits/riscv-mathvec.h: $(common-objpfx)config.make \
  $(..)sysdeps/riscv/rvv/math/Makefile \
  $(..)sysdeps/riscv/rvv/math/mathvec-variants.mk \
  $(..)sysdeps/riscv/rvv/math/riscv-mathvec.h.in \
  $(..)sysdeps/riscv/rvv/math/scripts/gen-mathvec-header.py \
  $(..)sysdeps/riscv/rvv/math/scripts/gen-mathvec-fortran.py
	$(make-target-directory)
	$(PYTHON) $(..)sysdeps/riscv/rvv/math/scripts/gen-mathvec-header.py \
	  --manifest $(..)sysdeps/riscv/rvv/math/mathvec-variants.mk \
	  --template $(..)sysdeps/riscv/rvv/math/riscv-mathvec.h.in \
	  --enabled $(if $(filter yes,$(build-mathvec)),yes,no) \
	  --output $@-tmp
	$(move-if-change) $@-tmp $@

headers += bits/riscv-mathvec-decls.h
generated += bits/riscv-mathvec-decls.h
before-compile += $(objpfx)bits/riscv-mathvec-decls.h
$(objpfx)bits/riscv-mathvec-decls.h: $(common-objpfx)config.make \
  $(..)sysdeps/riscv/rvv/math/Makefile \
  $(..)sysdeps/riscv/rvv/math/mathvec-variants.mk \
  $(..)sysdeps/riscv/rvv/math/riscv-mathvec-decls.h.in \
  $(..)sysdeps/riscv/rvv/math/scripts/gen-mathvec-header.py \
  $(..)sysdeps/riscv/rvv/math/scripts/gen-mathvec-fortran.py
	$(make-target-directory)
	$(PYTHON) $(..)sysdeps/riscv/rvv/math/scripts/gen-mathvec-header.py \
	  --manifest $(..)sysdeps/riscv/rvv/math/mathvec-variants.mk \
	  --decls-template $(..)sysdeps/riscv/rvv/math/riscv-mathvec-decls.h.in \
	  --enabled $(if $(filter yes,$(build-mathvec)),yes,no) \
	  --decls-output $@-tmp
	$(move-if-change) $@-tmp $@

# This versioned Fortran preinclude promises both fixed and scalable log m2.
# Only this lp64d sysdeps directory owns it in a shared multilib sysroot.
headers += finclude/math-vector-fortran-rvv-log-m2-v1.h
generated += finclude/math-vector-fortran-rvv-log-m2-v1.h
before-compile += $(objpfx)finclude/math-vector-fortran-rvv-log-m2-v1.h
$(objpfx)finclude/math-vector-fortran-rvv-log-m2-v1.h: \
  $(..)sysdeps/riscv/rvv/math/math-vector-fortran-rvv-log-m2-v1.h.in \
  $(common-objpfx)config.make
	$(make-target-directory)
ifeq ($(build-mathvec),yes)
	cp $< $@-tmp
else
	sed '/^!GCC/d' $< > $@-tmp
endif
	$(move-if-change) $@-tmp $@

# This versioned Fortran preinclude promises the five unmasked m2 families.
# It is generated from the manifest; a missing, duplicated or masked pair is
# a hard error.  The disabled form is comment-only and still installed.
# Only this lp64d sysdeps directory owns it in a shared multilib sysroot.
headers += finclude/math-vector-fortran-rvv-m2-v2.h
generated += finclude/math-vector-fortran-rvv-m2-v2.h
before-compile += $(objpfx)finclude/math-vector-fortran-rvv-m2-v2.h
$(objpfx)finclude/math-vector-fortran-rvv-m2-v2.h: \
  $(..)sysdeps/riscv/rvv/math/math-vector-fortran-rvv-m2-v2.h.in \
  $(..)sysdeps/riscv/rvv/math/mathvec-variants.mk \
  $(..)sysdeps/riscv/rvv/math/scripts/gen-mathvec-fortran.py \
  $(common-objpfx)config.make
	$(make-target-directory)
	$(PYTHON) $(..)sysdeps/riscv/rvv/math/scripts/gen-mathvec-fortran.py \
	  --manifest $(..)sysdeps/riscv/rvv/math/mathvec-variants.mk \
	  --template $(..)sysdeps/riscv/rvv/math/math-vector-fortran-rvv-m2-v2.h.in \
	  --enabled $(if $(filter yes,$(build-mathvec)),yes,no) \
	  --output $@-tmp
	$(move-if-change) $@-tmp $@

# This versioned Fortran preinclude carries the m1/m2 ABI masks.  It accepts
# partial m1 rows and emits only the exact bits present; the legacy v2 file
# above keeps its stricter m2 gate.
headers += finclude/math-vector-fortran-rvv-m1-v3.h
generated += finclude/math-vector-fortran-rvv-m1-v3.h
before-compile += $(objpfx)finclude/math-vector-fortran-rvv-m1-v3.h
$(objpfx)finclude/math-vector-fortran-rvv-m1-v3.h: \
  $(..)sysdeps/riscv/rvv/math/math-vector-fortran-rvv-m1-v3.h.in \
  $(..)sysdeps/riscv/rvv/math/mathvec-variants.mk \
  $(..)sysdeps/riscv/rvv/math/scripts/gen-mathvec-fortran.py \
  $(common-objpfx)config.make
	$(make-target-directory)
	$(PYTHON) $(..)sysdeps/riscv/rvv/math/scripts/gen-mathvec-fortran.py \
	  --manifest $(..)sysdeps/riscv/rvv/math/mathvec-variants.mk \
	  --template $(..)sysdeps/riscv/rvv/math/math-vector-fortran-rvv-m1-v3.h.in \
	  --contract m1-v3 \
	  --enabled $(if $(filter yes,$(build-mathvec)),yes,no) \
	  --output $@-tmp
	$(move-if-change) $@-tmp $@
ifeq ($(build-mathvec),yes)
libmvec-tests += $(sort $(foreach v,$(rvv-mathvec-variants),$(rvv-mathvec-$(v)-test)))
$(foreach v,$(rvv-mathvec-variants),$(eval $(rvv-mathvec-$(v)-test)-funcs += $(rvv-mathvec-$(v)-function)))
CFLAGS-test-double-rvv-m2-wrappers.c += -march=rv64gcv -mabi=lp64d
CFLAGS-test-double-rvv-m2-vla-wrappers.c += -march=rv64gcv -mabi=lp64d
CFLAGS-test-double-rvv-m2-mask-wrappers.c += -march=rv64gcv -mabi=lp64d
CFLAGS-test-double-rvv-m2-mask-vla-wrappers.c += -march=rv64gcv -mabi=lp64d
CFLAGS-test-double-rvv-m1-wrappers.c += -march=rv64gcv -mabi=lp64d -mrvv-max-lmul=m1
CFLAGS-test-double-rvv-m1-vla-wrappers.c += -march=rv64gcv -mabi=lp64d -mrvv-max-lmul=m1
CFLAGS-test-double-rvv-m1-mask-wrappers.c += -march=rv64gcv -mabi=lp64d -mrvv-max-lmul=m1
CFLAGS-test-double-rvv-m1-mask-vla-wrappers.c += -march=rv64gcv -mabi=lp64d -mrvv-max-lmul=m1
CFLAGS-test-double-rvv-m4-wrappers.c += -march=rv64gcv -mabi=lp64d -mrvv-max-lmul=m4
CFLAGS-test-double-rvv-m4-vla-wrappers.c += -march=rv64gcv -mabi=lp64d -mrvv-max-lmul=m4
CFLAGS-test-double-rvv-m8-wrappers.c += -march=rv64gcv -mabi=lp64d -mrvv-max-lmul=m8
CFLAGS-test-double-rvv-m8-vla-wrappers.c += -march=rv64gcv -mabi=lp64d -mrvv-max-lmul=m8

tests += tst-rvv-log
tests-static += tst-rvv-log-static
test-extras += tst-rvv-log-vector
extra-test-objs += tst-rvv-log-vector.o
CFLAGS-tst-rvv-log-vector.c += -march=rv64gcv -mabi=lp64d
$(objpfx)tst-rvv-log: $(objpfx)tst-rvv-log-vector.o $(libmvec)
$(objpfx)tst-rvv-log-static: $(objpfx)tst-rvv-log-vector.o $(common-objpfx)mathvec/libmvec.a $(common-objpfx)math/libm.a
$(objpfx)tst-rvv-log-static.o: $(..)sysdeps/riscv/rvv/math/tst-rvv-log.c
	$(compile-command.c)
endif
endif
