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

"""Generate versioned RISC-V Fortran SIMD contracts.

The downstream mathvec manifest is the single source of truth for exported
entries.  The v2 contract promises the five unmasked m2 double families
only after it has found the exact m2 fixed and scalable N pair for each of
them.  A missing, duplicated, masked, or otherwise mismatched m2 entry is a
hard error: v2 never silently advertises a subset of a promised family.

The v3 contract carries the per-function ABI masks introduced for m1.  It
validates every manifest row generically and emits only the bits actually
present, so a partial m1 library cannot advertise an absent LMUL/mode/mask.
Both contracts become comment-only when mathvec is disabled; they are still
installed so a compiler selecting a versioned file cannot fall through to an
older contract in the same sysroot.

The parser intentionally implements only the small, literal subset of make
syntax used by mathvec-variants.mk.  It does not expand variables or run a
shell, and it rejects malformed or duplicate assignments instead of guessing.
"""
import argparse
import pathlib
import re
import sys

PROGRAM = 'gen-mathvec-fortran.py'
MARKER = '@MATHVEC_FORTRAN_BUILTINS@'

# Contract scalar, manifest "function" base, precision, arity, fixed lanes.
# The scalable entry always uses lane marker "x".  v2 deliberately promises
# only m2 rows; m1 rows must be filtered by explicit LMUL metadata, and the
# exact _ZGV2 symbol check below must not be relaxed to accept _ZGV1 siblings.
CONTRACT = (
    ('log', 'log', 'double', 1, '4'),
    ('exp', 'exp', 'double', 1, '4'),
    ('sin', 'sin', 'double', 1, '4'),
    ('cos', 'cos', 'double', 1, '4'),
    ('pow', 'pow', 'double', 2, '4'),
)

ABI_FAMILIES = ('log', 'exp', 'sin', 'cos', 'pow')

# Scalars whose masked kernels are validated for the M ABI bits.  A
# scalar leaves this set only when the masked entry points, tests and
# exception behaviour are all verified; the header and the Fortran
# preinclude therefore never advertise an unverified masked clone.
MASKED_SCALARS = frozenset (('log', 'exp', 'sin', 'cos', 'pow'))
# Canonical function/precision/arity for every exported scalar.  These are the
# only values that may ever appear in a manifest row; deriving the expected
# symbol and fixed lane count from the same table prevents a self-consistent
# but semantically wrong row (for example a two-argument log) from passing.
SCALAR_CONTRACT = {
    scalar: (function, precision, str(arity))
    for scalar, function, precision, arity, _ in CONTRACT
}
# Scalars advertised by the frozen v3 per-function Fortran contract.
V3_FAMILIES = ('log', 'exp', 'sin', 'cos', 'pow')

# The v1 ABI mask has one nibble per LMUL: bits 0-3 LMUL1, 4-7 LMUL2,
# 8-11 LMUL4 and 12-15 LMUL8.  ABI_N_BITS/ABI_M_BITS are the union of every
# unmasked/masked bit the compiler table can encode, not the bits a particular
# installation promises.
ABI_N_BITS = 0x3333
ABI_M_BITS = 0xCCCC
# A compiler that predates the 16-bit mask rejects an argument above 255
# instead of ignoring the high nibbles.  The installed C header therefore
# offers the wider mask only behind __riscv_simd_math_abi_v1_lmul8, and the
# versioned Fortran preinclude keeps advertising the legacy LMUL 1/2 bits so
# that gfortran from either compiler generation stays usable.  A smaller mask
# promises a subset, which is always safe for a library that exports more.
ABI_LEGACY_BITS = 0xFF

ASSIGNMENT_RE = re.compile(
    r'^(?P<name>[A-Za-z0-9_.-]+)[ \t]*(?P<op>:=|\?=|\+=|=)[ \t]*(?P<value>.*)$')
VARIANT_RE = re.compile(r'^[A-Za-z0-9_.-]+$')


class ContractError(Exception):
    """The manifest or template cannot support the promised contract."""


def _strip_comment(line):
    """Remove an unescaped make comment from one logical line."""
    escaped = False
    for index, character in enumerate(line):
        if character == '#' and not escaped:
            return line[:index]
        if character == '\\':
            escaped = not escaped
        else:
            escaped = False
    return line


def _logical_lines(text):
    """Yield (line number, logical line) after joining make continuations."""
    pending = ''
    first = 1
    for number, physical in enumerate(text.splitlines(), 1):
        line = physical.rstrip('\r')
        if line.endswith('\\'):
            if not pending:
                first = number
            pending += line[:-1] + ' '
            continue
        if pending:
            yield first, pending + line
            pending = ''
        else:
            yield number, line
        first = number + 1
    if pending:
        yield first, pending


def parse_manifest(text):
    """Parse literal make assignments, rejecting anything ambiguous."""
    values = {}
    for number, logical in _logical_lines(text):
        line = _strip_comment(logical).strip()
        if not line:
            continue
        match = ASSIGNMENT_RE.match(line)
        if not match:
            raise ContractError(
                'line %d: not a literal make assignment: %r'
                % (number, logical))
        name = match.group('name')
        operator = match.group('op')
        value = match.group('value').strip()
        if '$' in value:
            raise ContractError(
                'line %d: variable references are not supported in %s'
                % (number, name))
        if operator == '+=':
            values[name] = (values.get(name, '') + ' ' + value).strip()
        elif operator == '?=':
            if name not in values:
                values[name] = value
        else:
            if name in values:
                raise ContractError(
                    'line %d: duplicate assignment for %s'
                    % (number, name))
            values[name] = value
    return values


def _field(values, variant, name):
    key = 'rvv-mathvec-%s-%s' % (variant, name)
    if key not in values:
        raise ContractError('%s is missing from the manifest' % key)
    value = values[key].strip()
    if not value:
        raise ContractError('%s is empty' % key)
    return value


def _rows(values):
    """Return all manifest rows with every structural field present.

    An empty variant list is a valid framework-only manifest: the generated
    headers then promise no entry and the build exports no vector symbol."""
    listing = values.get('rvv-mathvec-variants', '').split()
    seen = set()
    rows = []
    for variant in listing:
        if not VARIANT_RE.match(variant):
            raise ContractError('invalid variant name %r' % variant)
        if variant in seen:
            raise ContractError('duplicate variant %s' % variant)
        seen.add(variant)
        row = {
            'variant': variant,
            'function': _field(values, variant, 'function'),
            'scalar': _field(values, variant, 'scalar'),
            'precision': _field(values, variant, 'precision'),
            'arity': _field(values, variant, 'arity'),
            'lanes': _field(values, variant, 'lanes'),
            'lmul': _field(values, variant, 'lmul'),
            'mode': _field(values, variant, 'mode'),
            'routine': _field(values, variant, 'routine'),
            'symbol': _field(values, variant, 'symbol'),
            'version': _field(values, variant, 'version'),
            'capability': _field(values, variant, 'capability'),
            'availability': _field(values, variant, 'availability'),
            'test': _field(values, variant, 'test'),
            'mask': values.get('rvv-mathvec-%s-mask' % variant, 'N').strip(),
        }
        rows.append(row)
    return rows


def expected_fixed_lanes(precision, lmul):
    """Return the fixed lane count for one precision and LMUL."""
    base = 2 if precision == 'double' else 4
    return str(base * int(lmul))


def expected_symbol(scalar, arity, lanes, lmul, mask='N'):
    return '_ZGV%s%s%s%s_%s' % (lmul, mask, lanes, 'v' * arity, scalar)


# The manifest may describe any LMUL the ABI mask can encode.  Only LMUL 1
# and 2 are wired into the frozen v2 Fortran contract below; the wider LMULs
# reach callers through the generated C header.
ABI_LMULS = (1, 2, 4, 8)


def abi_bit(row):
    """Return the v1 ABI bit for one precise manifest variant."""
    lmul = int(row['lmul'])
    if lmul not in ABI_LMULS:
        raise ContractError('%s: lmul is %r, expected one of 1, 2, 4, 8'
                            % (row['variant'], row['lmul']))
    if row['mode'] not in ('fixed', 'vla'):
        raise ContractError('%s: unsupported mode %r'
                            % (row['variant'], row['mode']))
    if row['mask'] not in ('N', 'M'):
        raise ContractError('%s: unsupported mask convention %r'
                            % (row['variant'], row['mask']))
    # One nibble per LMUL: log2 (1, 2, 4, 8) selects bits 0, 4, 8, 12.
    shift = 4 * (lmul.bit_length () - 1)
    shift += 0 if row['mode'] == 'fixed' else 1
    shift += 0 if row['mask'] == 'N' else 2
    return 1 << shift


def validate_rows(rows):
    """Validate exact scalar semantics and reject duplicate ABI bits."""
    seen = {}
    for row in rows:
        where = row['variant']
        try:
            lmul = int(row['lmul'])
        except ValueError:
            raise ContractError('%s: lmul is %r, expected one of 1, 2, 4, 8'
                                % (where, row['lmul']))
        if lmul not in ABI_LMULS:
            raise ContractError('%s: lmul is %r, expected one of 1, 2, 4, 8'
                                % (where, row['lmul']))
        expected = SCALAR_CONTRACT.get(row['scalar'])
        if expected is None:
            raise ContractError('%s: unsupported scalar %r'
                                % (where, row['scalar']))
        expected_function, expected_precision, expected_arity = expected
        if row['function'] != expected_function:
            raise ContractError(
                '%s: function is %r, expected %r for scalar %s'
                % (where, row['function'], expected_function,
                   row['scalar']))
        if row['precision'] not in ('float', 'double'):
            raise ContractError('%s: unsupported precision %r'
                                % (where, row['precision']))
        if row['precision'] != expected_precision:
            raise ContractError(
                '%s: precision is %r, expected %r for %s'
                % (where, row['precision'], expected_precision,
                   row['scalar']))
        if row['arity'] not in ('1', '2'):
            raise ContractError('%s: unsupported arity %r'
                                % (where, row['arity']))
        if row['arity'] != expected_arity:
            raise ContractError(
                '%s: arity is %r, expected %s for %s'
                % (where, row['arity'], expected_arity, row['scalar']))
        if row['mode'] not in ('fixed', 'vla'):
            raise ContractError('%s: unsupported mode %r'
                                % (where, row['mode']))
        if row['mask'] not in ('N', 'M'):
            raise ContractError('%s: unsupported mask convention %r'
                                % (where, row['mask']))
        # Only validated masked kernels may contribute M capability bits.
        if row['mask'] == 'M' and row['scalar'] not in MASKED_SCALARS:
            raise ContractError(
                '%s: masked (M) ABI is not validated for %s'
                % (where, row['scalar']))
        lanes = (expected_fixed_lanes(expected_precision, lmul)
                 if row['mode'] == 'fixed' else 'x')
        if row['lanes'] != lanes:
            raise ContractError(
                '%s: lanes is %r, expected %r for %s lmul%s %s entry'
                % (where, row['lanes'], lanes, expected_precision,
                   row['lmul'], row['mode']))
        symbol = expected_symbol(row['scalar'], int(expected_arity),
                                 lanes, row['lmul'], row['mask'])
        if row['symbol'] != symbol:
            raise ContractError(
                '%s: symbol is %r, expected %r for %s'
                % (where, row['symbol'], symbol, row['scalar']))
        if not row['test'].startswith(expected_precision + '-'):
            raise ContractError('%s: test %r does not match precision %s'
                                % (where, row['test'], expected_precision))
        key = (row['scalar'], lmul, row['mode'], row['mask'])
        if key in seen:
            raise ContractError(
                '%s: duplicate ABI bit for %s lmul%d %s %s entry '
                '(already present as %s)'
                % (where, row['scalar'], lmul, row['mode'], row['mask'],
                   seen[key]))
        seen[key] = where
    return rows


def parse_rows(text):
    """Parse the manifest and return validated rows for every variant."""
    return validate_rows(_rows(parse_manifest(text)))


def function_masks(rows):
    """Return the OR of every precise row's ABI bit, per function."""
    masks = {scalar: 0 for scalar in ABI_FAMILIES}
    for row in rows:
        if row['scalar'] not in masks:
            raise ContractError('%s: unsupported scalar %r'
                                % (row['variant'], row['scalar']))
        bit = abi_bit(row)
        if masks[row['scalar']] & bit:
            raise ContractError(
                '%s: duplicate ABI bit for %s (already promised by another '
                'manifest row)' % (row['variant'], row['scalar']))
        masks[row['scalar']] |= bit
    return masks


def validate_contract(rows):
    """Return the verified fixed/scalable pair for each promised scalar."""
    # A manifest without rows describes a framework-only tree: it promises
    # nothing, so the preinclude carries comments only.  Every non-empty
    # manifest still has to satisfy the full contract below.
    if not rows:
        return []
    promised = []
    for scalar, function, precision, arity, fixed_lanes in CONTRACT:
        family = []
        for row in rows:
            if row['lmul'] != '2' or row['scalar'] != scalar:
                continue
            if row['mask'] != 'N':
                continue
            where = row['variant']
            if row['function'] != function:
                raise ContractError(
                    '%s: function is %r, expected %r for scalar %s'
                    % (where, row['function'], function, scalar))
            if row['precision'] != precision:
                raise ContractError(
                    '%s: precision is %r, expected %r for %s'
                    % (where, row['precision'], precision, scalar))
            if row['arity'] != str(arity):
                raise ContractError(
                    '%s: arity is %r, expected %d for %s'
                    % (where, row['arity'], arity, scalar))
            if row['mode'] not in ('fixed', 'vla'):
                raise ContractError(
                    '%s: unsupported mode %r for %s'
                    % (where, row['mode'], scalar))
            lanes = fixed_lanes if row['mode'] == 'fixed' else 'x'
            if row['lanes'] != lanes:
                raise ContractError(
                    '%s: lanes is %r, expected %r for %s %s entry'
                    % (where, row['lanes'], lanes, scalar, row['mode']))
            symbol = expected_symbol(scalar, arity, lanes, '2')
            if row['symbol'] != symbol:
                raise ContractError(
                    '%s: symbol is %r, expected %r for %s'
                    % (where, row['symbol'], symbol, scalar))
            family.append(row)

        fixed = [row for row in family if row['mode'] == 'fixed']
        scalable = [row for row in family if row['mode'] == 'vla']
        if len(fixed) != 1 or len(scalable) != 1:
            raise ContractError(
                '%s: expected exactly one unmasked m2 fixed and one unmasked '
                'm2 scalable manifest entry, found %d fixed and %d scalable'
                % (scalar, len(fixed), len(scalable)))
        promised.append({
            'scalar': scalar,
            'fixed_symbol': fixed[0]['symbol'],
            'scalable_symbol': scalable[0]['symbol'],
        })
    return promised


def _builtins(promised):
    if not promised:
        return ['! No unmasked m2 entry is promised by this manifest.']
    comments = ['! Unmasked m2 entries verified in mathvec-variants.mk:']
    for entry in promised:
        comments.append('! %s: %s, %s'
                        % (entry['scalar'], entry['fixed_symbol'],
                           entry['scalable_symbol']))
    directives = [
        "!GCC$ builtin (%s) attributes simd (notinbranch) if('fastmath')"
        % entry['scalar']
        for entry in promised
    ]
    return comments + directives


def _v3_builtins(masks):
    comments = ['! Per-function ABI masks verified in mathvec-variants.mk:']
    for scalar in V3_FAMILIES:
        mask = masks[scalar] & ABI_LEGACY_BITS
        if mask:
            comments.append('! %s: riscv_simd_math_abi_v1(%d)'
                            % (scalar, mask))
        else:
            comments.append('! %s: no manifest entry (mask 0)' % scalar)
    comments.append('! The directive mask keeps the legacy eight-bit LMUL 1/2 '
                    'subset so that a gfortran that predates the 16-bit mask '
                    'still accepts it; the installed C header carries the '
                    'wider mask behind __riscv_simd_math_abi_v1_lmul8.')
    directives = []
    for scalar in V3_FAMILIES:
        mask = masks[scalar] & ABI_LEGACY_BITS
        if mask & ABI_N_BITS:
            directives.append(
                "!GCC$ builtin (%s) attributes simd (notinbranch) "
                "riscv_simd_math_abi_v1(%d) if('fastmath')"
                % (scalar, mask))
        if mask & ABI_M_BITS:
            directives.append(
                "!GCC$ builtin (%s) attributes simd (inbranch) "
                "riscv_simd_math_abi_v1(%d) if('fastmath')"
                % (scalar, mask))
    return comments + directives


def _substitute(template, replacement):
    """Substitute the single marker; reject templates that pre-set mappings."""
    lines = template.splitlines()
    matches = [index for index, line in enumerate(lines) if MARKER in line]
    if len(matches) != 1 or lines[matches[0]].strip() != MARKER:
        raise ContractError(
            'template must contain exactly one standalone %s marker line'
            % MARKER)
    if any(line.lstrip().startswith('!GCC$') for line in lines):
        raise ContractError(
            'template must not contain builtin directives; they are generated')
    index = matches[0]
    result = lines[:index] + replacement + lines[index + 1:]
    return '\n'.join(result) + '\n'


def render(template, enabled, promised):
    """Substitute the single marker; never leave a stale or partial promise."""
    if enabled:
        replacement = _builtins(promised)
    else:
        replacement = [
            '! mathvec is disabled in this build: no SIMD math mappings '
            'are promised.'
        ]
    return _substitute(template, replacement)


def render_v3(template, enabled, masks):
    """Substitute v3's marker with only the ABI bits present in the library."""
    if enabled:
        replacement = _v3_builtins(masks)
    else:
        replacement = [
            '! mathvec is disabled in this build: no SIMD math mappings '
            'are promised.'
        ]
    return _substitute(template, replacement)


def generate(manifest_text, template_text, enabled, contract='m2'):
    rows = _rows(parse_manifest(manifest_text))
    if contract == 'm2':
        # Keep the historical v2 error ordering: a missing unmasked pair is a
        # contract failure, not a generic symbol failure on some other row.
        promised = validate_contract(rows)
        validate_rows(rows)
        return render(template_text, enabled, promised)
    if contract == 'm1-v3':
        validate_rows(rows)
        masks = function_masks(rows)
        return render_v3(template_text, enabled, masks)
    raise ContractError('unknown contract %r' % contract)


def main(argv=None):
    parser = argparse.ArgumentParser(
        description='Generate a versioned RISC-V Fortran SIMD contract.')
    parser.add_argument('--manifest', required=True, type=pathlib.Path,
                        help='path to mathvec-variants.mk')
    parser.add_argument('--template', type=pathlib.Path,
                        help='comment template with the builtins marker')
    parser.add_argument('--contract', choices=('m2', 'm1-v3'), default='m2',
                        help='m2 keeps the legacy five-pair contract; m1-v3 '
                        'emits per-function ABI masks from actual rows')
    parser.add_argument('--enabled', required=True, choices=('yes', 'no'),
                        help='yes emits directives; no emits comments only')
    parser.add_argument('--output', type=pathlib.Path,
                        help='write here instead of standard output')
    args = parser.parse_args(argv)
    template = args.template
    if template is None:
        name = ('math-vector-fortran-rvv-m1-v3.h.in'
                if args.contract == 'm1-v3'
                else 'math-vector-fortran-rvv-m2-v2.h.in')
        template = pathlib.Path(__file__).resolve().parent / name

    try:
        manifest_text = args.manifest.read_text(encoding='utf-8')
        template_text = template.read_text(encoding='utf-8')
        result = generate(manifest_text, template_text,
                          args.enabled == 'yes', args.contract)
    except (ContractError, OSError) as error:
        sys.stderr.write('%s: error: %s\n' % (PROGRAM, error))
        return 2

    if args.output is not None:
        try:
            args.output.write_text(result, encoding='utf-8')
        except OSError as error:
            sys.stderr.write('%s: error: %s\n' % (PROGRAM, error))
            return 2
    else:
        sys.stdout.write(result)
    return 0


if __name__ == '__main__':
    sys.exit(main())
