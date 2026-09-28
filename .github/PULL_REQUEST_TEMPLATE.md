## What

<!-- One or two sentences. -->

## Why

<!-- Erratum, test bug, reference, exercise…? Link the issue if there is one. -->

## Checklist

- [ ] No solutions to stubbed systems are included
- [ ] `tools/armory.sh` runs; C builds with `-Werror`
- [ ] `python3 tools/refs/generate.py --check` passes (if references changed)
- [ ] `python3 site/build.py && mkdocs build --strict -f site/mkdocs.yml` passes (if docs changed)
- [ ] Test changes: validated against a private correct implementation, the stubs, and a planted bug (describe below)

## Evidence

<!-- Pass counts, plots, derivations. -->
