# Contributing

`tony-stark` is a personal self-study lab, made public so others can follow the same path. Contributions that make the curriculum **more correct, clearer or better referenced** are very welcome.

## Welcome contributions

| Kind | How |
|---|---|
| **Errata**: a wrong equation, claim, citation or broken link | open an [erratum issue](https://github.com/Lourdhu02/tony-stark/issues/new?template=erratum.yml), or a PR with the fix |
| **Test bugs**: a test that passes a wrong implementation, or fails a correct one | open a [test-bug issue](https://github.com/Lourdhu02/tony-stark/issues/new?template=test-bug.yml) with a minimal reproduction |
| **References**: a landmark paper that belongs in a folder | open a [paper suggestion](https://github.com/Lourdhu02/tony-stark/issues/new?template=paper.yml), or edit `tools/refs/catalog.py` and regenerate |
| **Exercises and stretch goals** | a PR to the relevant chapter, manual or system README |

## Not accepted

- **Solutions to the stubbed systems.** This is the one hard rule. The value of the lab is implementing them yourself. PRs that fill in `src/` stubs or the `blueprint` learner modules will be closed. If you've solved a system, great: keep it in your fork.
- Unverified citations. Every reference must exist, with correct authors, year and venue.

## Before opening a PR

```sh
tools/armory.sh                                   # all systems build (-Werror) and the dashboard runs
make SAN=1 -C marks/mark-01-box-of-scraps test    # sanitizers
python3 tools/refs/generate.py --check            # generated REFERENCES.md are fresh
python3 site/build.py && mkdocs build --strict -f site/mkdocs.yml   # the docs site builds
```

**If you change a test suite, show your evidence in the PR:**
- it passes against a correct implementation (keep that private; don't commit it)
- it fails against the stubs
- it catches at least one planted bug

## Style

- Math conventions: [`chapters/00-notation.md`](marks/mark-11-blueprint/chapters/00-notation.md). Twists are (ω, v), angular first.
- Math renders on GitHub: fenced `math` blocks for display math; `` $`…`$ `` for inline math containing `\\`.
- Docs voice: precise, terse, terminal-flavoured (`### \`> command\``).
- Commits: an imperative subject line (≤ 72 chars), with a body explaining *why*.

By contributing, you agree that your contributions are licensed under the repo's licenses: MIT for code, CC BY 4.0 for content (see [LICENSE-CONTENT.md](LICENSE-CONTENT.md)). Please follow the [Code of Conduct](CODE_OF_CONDUCT.md).
