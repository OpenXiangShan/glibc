ifeq ($(subdir),misc)
sysdep_headers += sys/asm.h
endif

ifeq ($(subdir),elf)
gen-as-const-headers += dl-link.sym

ifeq ($(build-shared),yes)
ifeq ($(have-ifunc),yes)
tests += tst-variant-cc tst-variant-cc-dlopen tst-variant-cc-audit
modules-names += tst-variant-cc-mod tst-variant-cc-dlopenmod tst-variant-cc-auditmod
LDFLAGS-tst-variant-cc-audit += -Wl,-z,lazy
$(objpfx)tst-variant-cc-audit: $(objpfx)tst-variant-cc-mod.so
$(objpfx)tst-variant-cc-audit.out: $(objpfx)tst-variant-cc-auditmod.so
tst-variant-cc-audit-ENV = LD_AUDIT=$(objpfx)tst-variant-cc-auditmod.so
LDFLAGS-tst-variant-cc += -Wl,-z,lazy
LDFLAGS-tst-variant-cc-dlopenmod.so += -Wl,-z,lazy
$(objpfx)tst-variant-cc-dlopen: $(shared-thread-library)
$(objpfx)tst-variant-cc-dlopen.out: $(objpfx)tst-variant-cc-dlopenmod.so
$(objpfx)tst-variant-cc-dlopenmod.so: $(objpfx)tst-variant-cc-mod.so
$(objpfx)tst-variant-cc: $(objpfx)tst-variant-cc-mod.so
$(objpfx)tst-variant-cc-mod.so: map-file = \
  $(..)sysdeps/riscv/tst-variant-cc-mod.map
$(objpfx)tst-variant-cc-mod.so: \
  $(..)sysdeps/riscv/tst-variant-cc-mod.map
endif
endif
endif

# RISC-V's assembler also needs to know about PIC as it changes the definition
# of some assembler macros.
ASFLAGS-.os += $(pic-ccflag)

ifeq (no,$(riscv-r-align))
ASFLAGS-.os += -Wa,-mno-relax
ASFLAGS-.o += -Wa,-mno-relax
sysdep-CFLAGS += -mno-relax
endif
