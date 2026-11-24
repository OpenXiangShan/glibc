# RVV binary64 kernel provenance

## log, exp, sin, cos and pow

The register kernels and constant data originate from rvv-bench revision
`b63f4ae6a54d162b362664f24e55a4b430a9a0c5` under
`single/libmvec/oldarchive/`.

| RVV function | rvv-bench source | Original GNU libc algorithm |
| --- | --- | --- |
| log | `log/logvec_impl.inc.h`, `log/logvec_data.c` | `dbl-64/e_log.c` and `e_log_data.c` with a 128-bin reduction and a dedicated near-one polynomial |
| exp | `exp/expvec_impl.inc.h`, `exp/expvec_data.c` | `dbl-64/e_exp.c` and `e_exp_data.c` with a 128-bin scale and tail table |
| sin and cos | `common/trigvec_impl.h`, `common/trigvec_data.c` | IBM AML `dbl-64/s_sin.c`, `usncs.h` and `sincostab.c` with split pi/2 reduction and a shared table |
| pow | `pow/powvec_impl.inc.h`, `pow/powvec_data.h` | `dbl-64/e_pow.c`, `e_pow_log_data.c` and `e_exp_data.c` with a compensated logarithm and product followed by exp reconstruction |

These sources carry GNU libc's LGPL-2.1-or-later terms. The library uses
register-only cores and its fixed, scalable and masked wrappers. The old
benchmark array wrappers are omitted. The log, exp and pow ports use the
benchmark's dispatch to cold routines. The log port uses the m1 multiply-add
operand ordering from the benchmark. The sin/cos port uses the baseline
kernel. Cold lanes use scalar libm. The ABI adapters select nearest rounding
for the shift-based reductions, preserve accrued exception flags and restore
the caller's rounding mode. Inactive lanes are replaced in the integer domain
with 1 for log, 0 for exp/sin/cos, and (1, 1) for pow.
