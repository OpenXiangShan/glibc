# The downstream RISC-V mathvec implementation is built for RVV separately
# from the baseline rv64gc libc.  This directory is selected only for lp64d.
include $(..)sysdeps/riscv/rvv/math/mathvec-variants.mk

ifeq ($(subdir),mathvec)
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

endif
