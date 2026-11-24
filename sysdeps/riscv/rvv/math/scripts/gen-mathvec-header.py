#!/usr/bin/env python3
# Copyright (C) 2026 Free Software Foundation, Inc.
# This file is part of the GNU C Library.
#
# The GNU C Library is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2.1 of the License, or (at your option) any later version.
#
# The GNU C Library is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public
# License along with the GNU C Library; if not, see
# <https://www.gnu.org/licenses/>.

"""Generate bits/riscv-mathvec.h and bits/riscv-mathvec-decls.h from the
mathvec manifest.

Availability macros retain their historical names.  In addition, every
function gets one mask macro containing the OR of the ABI bits for the exact
manifest rows that the installed library promises.  A disabled build defines
all masks as zero; a partial build leaves absent bits clear.  The mask macro
is emitted with two definitions: the full 16-bit LMUL1/2/4/8 mask is selected
only when the compiler advertises __riscv_simd_math_abi_v1_lmul8, because an
older compiler rejects an attribute argument above 255 instead of ignoring the
LMUL4/LMUL8 nibbles; otherwise the historical eight-bit LMUL1/2 subset is
offered.  The companion math-vector.h only takes its attribute path when these
mask macros exist.
"""
import argparse
import importlib.util
import pathlib
import re
import sys

PROGRAM = 'gen-mathvec-header.py'
MARKER = '@MATHVEC_C_DEFINES@'
DECLS_MARKER = '@MATHVEC_DECLS@'

# Declaration blocks keep the historical order first so a regenerated header
# stays textually identical to the hand-written one, then follow manifest
# order for anything new.  The order is cosmetic; the mask decides the content.
DECL_ORDER = ('log', 'exp', 'sin', 'cos', 'pow')
# Functions whose masked family also has the private length-first ABI marker.
LEN_ATTR_FUNCTIONS = ('log', 'exp')

ABI_BITS = (
    (0x01, 'm1 fixed-N'),
    (0x02, 'm1 Nx-N'),
    (0x04, 'm1 fixed-M'),
    (0x08, 'm1 Nx-M'),
    (0x10, 'm2 fixed-N'),
    (0x20, 'm2 Nx-N'),
    (0x40, 'm2 fixed-M'),
    (0x80, 'm2 Nx-M'),
    (0x100, 'm4 fixed-N'),
    (0x200, 'm4 Nx-N'),
    (0x400, 'm4 fixed-M'),
    (0x800, 'm4 Nx-M'),
    (0x1000, 'm8 fixed-N'),
    (0x2000, 'm8 Nx-N'),
    (0x4000, 'm8 fixed-M'),
    (0x8000, 'm8 Nx-M'),
)
# A compiler that predates the 16-bit mask rejects an attribute argument above
# 255 instead of ignoring the high nibbles, so the generated header offers the
# wide value only behind the matching capability macro and falls back to the
# LMUL 1/2 subset otherwise.  The subset never advertises more than the library
# exports.
WIDE_MASK_GUARD = '__riscv_simd_math_abi_v1_lmul8'
LEGACY_MASK_BITS = 0xFF


def load_contract_generator():
    path = pathlib.Path(__file__).resolve().parent / 'gen-mathvec-fortran.py'
    spec = importlib.util.spec_from_file_location('mathvec_contract', str(path))
    if spec is None or spec.loader is None:
        raise SystemExit('cannot load ' + str(path))
    module = importlib.util.module_from_spec(spec)
    try:
        spec.loader.exec_module(module)
    except (ImportError, OSError, SyntaxError) as error:
        raise SystemExit('cannot load ' + str(path) + ': ' + str(error))
    return module


def build_defines(contract, rows, masks, enabled):
    """Return the macro lines for the generated header."""
    lines = []
    overall = 1 if enabled else 0
    lines.append('#define __GLIBC_RISCV_MATHVEC %d' % overall)

    availability = {'__GLIBC_RISCV_MATHVEC': overall}
    for row in rows:
        name = row['availability']
        value = 1 if enabled else 0
        if name in availability and availability[name] != value:
            raise contract.ContractError(
                '%s: conflicting availability macro %s'
                % (row['variant'], name))
        availability[name] = value
    for name in sorted(availability):
        if name == '__GLIBC_RISCV_MATHVEC':
            continue
        lines.append('#define %s %d' % (name, availability[name]))

    lines.append('')
    for scalar in contract.ABI_FAMILIES:
        mask = masks[scalar] if enabled else 0
        bits = [label for bit, label in ABI_BITS if mask & bit]
        if bits:
            lines.append('/* %s: %s */' % (scalar, ', '.join(bits)))
        else:
            lines.append('/* %s: no entries (mask 0) */' % scalar)
        macro = '__GLIBC_RISCV_MATHVEC_%s_ABI_MASK' % scalar.upper()
        wide = mask
        legacy = mask & LEGACY_MASK_BITS
        lines.append('#if defined %s' % WIDE_MASK_GUARD)
        lines.append('# define %s %d' % (macro, wide))
        lines.append('#else')
        lines.append('# define %s %d' % (macro, legacy))
        lines.append('#endif')
    return lines


def ordered_scalars(contract, rows):
    """Return the scalars to emit, historical order first, then manifest order."""
    present = []
    for row in rows:
        if row['scalar'] not in present:
            present.append(row['scalar'])
    known = [scalar for scalar in DECL_ORDER if scalar in present]
    extra = [scalar for scalar in present if scalar not in DECL_ORDER]
    for scalar in contract.ABI_FAMILIES:
        if scalar in present and scalar not in known and scalar not in extra:
            extra.append(scalar)
    ordered = known + extra
    missing = [scalar for scalar in present if scalar not in ordered]
    if missing:
        raise contract.ContractError(
            'no declaration order for %s' % ', '.join(sorted(missing)))
    return ordered


def build_decls(contract, rows, masks, enabled):
    """Return the __DECL_SIMD_* block lines for the generated declarations."""
    if not enabled:
        return ['/* mathvec is disabled: no vector declarations are promised.  */']
    lines = []
    for scalar in ordered_scalars(contract, rows):
        mask = masks[scalar]
        if not mask:
            continue
        upper = scalar.upper()
        mask_macro = '__GLIBC_RISCV_MATHVEC_%s_ABI_MASK' % upper
        len_attr = (scalar in LEN_ATTR_FUNCTIONS and mask & contract.ABI_M_BITS)
        tail = ''
        if len_attr:
            tail = (' \\\n  __GLIBC_RISCV_MATHVEC_%s_LEN_ATTR'
                    % upper)
        blocks = []
        for kind in ('NM', 'N', 'M'):
            if kind == 'NM':
                blocks.append(
                    '# if ((%s) & __GLIBC_RISCV_MATHVEC_ABI_N_BITS) \\\n'
                    '      && ((%s) & __GLIBC_RISCV_MATHVEC_ABI_M_BITS)'
                    % (mask_macro, mask_macro))
            else:
                blocks.append(
                    '# elif ((%s) & __GLIBC_RISCV_MATHVEC_ABI_%s_BITS)'
                    % (mask_macro, kind))
            blocks.append('#  undef __DECL_SIMD_%s' % scalar)
            blocks.append('#  define __DECL_SIMD_%s \\\n'
                          '  __GLIBC_RISCV_MATHVEC_ABI_%s_ATTR (%s)%s'
                          % (scalar, kind, mask_macro, tail))
        blocks.append('# endif')
        lines.extend(blocks)
        lines.append('')
    return lines


def verify_export_set(contract, manifest_path, rows):
    """Fail the build when the manifest disagrees with the exported ABI.

    The real source tree keeps Versions, libmvec.abilist and the generic-math
    adapters next to the manifest, so every build re-checks that the manifest,
    the version script, the ABI list and the wrappers describe the same
    symbols.  A manifest copied elsewhere for a standalone run has no such
    siblings and skips the check.
    """
    directory = manifest_path.resolve().parent
    abilist = directory / 'libmvec.abilist'
    versions = directory / 'Versions'
    if not abilist.is_file() or not versions.is_file():
        return

    expected = {(row['version'], row['symbol']) for row in rows}
    if len(expected) != len(rows):
        raise contract.ContractError(
            'manifest lists %d variants but only %d distinct ABI symbols'
            % (len(rows), len(expected)))

    abi_pairs = set(re.findall(r'^(GLIBC_\S+)\s+(_ZGV\S+)\s+F\s*$',
                               abilist.read_text(encoding='utf-8'), re.M))
    version_pairs = set()
    current = None
    for line in versions.read_text(encoding='utf-8').splitlines():
        match = re.search(r'\b(GLIBC_\d+\.\d+)\s*\{', line)
        if match:
            current = match.group(1)
        for symbol in re.findall(r'(_ZGV\w+)\s*;', line):
            version_pairs.add((current, symbol))

    for name, found in (('libmvec.abilist', abi_pairs),
                        ('Versions', version_pairs)):
        if found != expected:
            missing = sorted(expected - found)
            extra = sorted(found - expected)
            raise contract.ContractError(
                '%s and mathvec-variants.mk disagree: missing %r, unexpected %r'
                % (name, missing[:4], extra[:4]))

    for row in rows:
        routine = directory / (row['routine'] + '.c')
        wrapper = directory / ('test-' + row['test'] + '-wrappers.c')
        if not routine.is_file():
            raise contract.ContractError(
                '%s: missing routine source %s' % (row['variant'], routine.name))
        if not re.search(r'\b' + re.escape(row['symbol']) + r'\s*\(',
                         routine.read_text(encoding='utf-8')):
            raise contract.ContractError(
                '%s: %s does not define %s'
                % (row['variant'], routine.name, row['symbol']))
        if not wrapper.is_file():
            raise contract.ContractError(
                '%s: missing generic-math adapter %s'
                % (row['variant'], wrapper.name))
        if row['symbol'] not in wrapper.read_text(encoding='utf-8'):
            raise contract.ContractError(
                '%s: %s does not call %s'
                % (row['variant'], wrapper.name, row['symbol']))


def render(manifest_text, template_text, enabled):
    contract = load_contract_generator()
    rows = contract.parse_rows(manifest_text)
    masks = contract.function_masks(rows)
    defines = build_defines(contract, rows, masks, enabled)

    lines = template_text.splitlines()
    matches = [index for index, line in enumerate(lines) if MARKER in line]
    if len(matches) != 1 or lines[matches[0]].strip() != MARKER:
        raise contract.ContractError(
            'template must contain exactly one standalone %s marker line'
            % MARKER)
    index = matches[0]
    result = lines[:index] + defines + lines[index + 1:]
    return '\n'.join(result) + '\n'


def render_decls(manifest_text, template_text, enabled):
    contract = load_contract_generator()
    rows = contract.parse_rows(manifest_text)
    masks = contract.function_masks(rows)
    decls = build_decls(contract, rows, masks, enabled)

    lines = template_text.splitlines()
    matches = [index for index, line in enumerate(lines)
               if DECLS_MARKER in line]
    if len(matches) != 1 or lines[matches[0]].strip() != DECLS_MARKER:
        raise contract.ContractError(
            'template must contain exactly one standalone %s marker line'
            % DECLS_MARKER)
    index = matches[0]
    result = lines[:index] + decls + lines[index + 1:]
    text = '\n'.join(result)
    if not text.endswith('\n'):
        text += '\n'
    return text


def main(argv=None):
    parser = argparse.ArgumentParser(
        description='Generate bits/riscv-mathvec.h from mathvec-variants.mk.')
    parser.add_argument('--manifest', required=True, type=pathlib.Path)
    parser.add_argument('--template', type=pathlib.Path,
                        help='availability template for bits/riscv-mathvec.h')
    parser.add_argument('--output', type=pathlib.Path,
                        help='write availability header here instead of stdout')
    parser.add_argument('--decls-template', type=pathlib.Path,
                        help='template for bits/riscv-mathvec-decls.h')
    parser.add_argument('--decls-output', type=pathlib.Path,
                        help='write declaration header here instead of stdout')
    parser.add_argument('--enabled', required=True, choices=('yes', 'no'))
    parser.add_argument('--no-export-check', action='store_true',
                        help='skip the manifest/Versions/abilist/wrapper check')
    args = parser.parse_args(argv)

    if args.template is None and args.decls_template is None:
        sys.stderr.write('%s: error: specify --template or --decls-template\n'
                         % PROGRAM)
        return 2
    if args.output is not None and args.template is None:
        sys.stderr.write('%s: error: --output needs --template\n' % PROGRAM)
        return 2
    if args.decls_output is not None and args.decls_template is None:
        sys.stderr.write('%s: error: --decls-output needs --decls-template\n'
                         % PROGRAM)
        return 2

    contract = load_contract_generator()
    enabled = args.enabled == 'yes'
    try:
        manifest_text = args.manifest.read_text(encoding='utf-8')
        if not args.no_export_check:
            verify_export_set(contract, args.manifest,
                              contract.parse_rows(manifest_text))
        if args.template is not None:
            availability = render(manifest_text,
                                  args.template.read_text(encoding='utf-8'),
                                  enabled)
        else:
            availability = None
        if args.decls_template is not None:
            decls = render_decls(manifest_text,
                                 args.decls_template.read_text(encoding='utf-8'),
                                 enabled)
        else:
            decls = None
    except (contract.ContractError, OSError) as error:
        sys.stderr.write('%s: error: %s\n' % (PROGRAM, error))
        return 2

    try:
        if availability is not None:
            if args.output is not None:
                args.output.write_text(availability, encoding='utf-8')
            else:
                sys.stdout.write(availability)
        if decls is not None:
            if args.decls_output is not None:
                args.decls_output.write_text(decls, encoding='utf-8')
            else:
                sys.stdout.write(decls)
    except OSError as error:
        sys.stderr.write('%s: error: %s\n' % (PROGRAM, error))
        return 2
    return 0


if __name__ == '__main__':
    sys.exit(main())
