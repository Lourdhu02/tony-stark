# CLAUDE.md

Guidance for Claude Code (and any AI assistant) working in this repository.

## What this repo is

`tony-stark` is a **self-study lab**. Its owner learns systems, robotics and AI by building each piece from scratch, one "Mark" at a time. The repo is a curriculum, not a product. **Its most important property is that the learner does the learning.**

## The prime directive: never write the learner's solutions

- Files under `marks/*/**/src/` (C) and `marks/*/lab/blueprint/{so3,se3,kinematics,dynamics}.py` contain **stubs the learner must implement**. Never fill them in, never paste a working implementation, and never show equivalent code in chat, **unless the learner explicitly says they have finished that system and want a reference comparison.**
- When the learner is stuck, follow the **hint ladder** in the system's README (nudge → approach → algorithm) and the unstuck order in `docs/method.md`. Ask questions before giving answers.
- Explaining a *concept*, deriving math, debugging *their* code by pointing at the faulty line and explaining why, or writing *new* tests: all fine. Writing the missing function: not fine.
- Use the subagents in `.claude/agents/` (`tutor`, `reviewer`, `quizmaster`, `paper-guide`, `lab-scribe`) when they fit.

## Layout

| Path | What |
|---|---|
| `marks/mark-NN-*/README.md` | Mark dossiers: syllabus, systems, quiz, boss fight |
| `marks/mark-01-box-of-scraps/0N-*/` | C systems: `include/` (contract), `src/` (stubs + given files), `tests/` |
| `marks/mark-11-blueprint/` | Python lab (`lab/blueprint`) + textbook chapters (`chapters/`) |
| `*/REFERENCES.md`, `docs/papers.md` | **Generated** from `tools/refs/catalog.py`. Never edit them by hand. |
| `tools/armory.sh` | test runner + dashboard; reads `RESULT <pass> <total>` lines |
| `docs/` | method, skill tree, library, self-assessment |
| `site/` | MkDocs config + build script for the GitHub Pages site |

## Commands

```sh
make -C marks/mark-01-box-of-scraps test     # C systems (dashboard)
make -C marks/mark-11-blueprint test         # Python systems (needs numpy, pytest)
make SAN=1 -C marks/mark-01-box-of-scraps test
tools/armory.sh                               # everything
python3 tools/refs/generate.py               # regenerate REFERENCES.md files
python3 tools/refs/generate.py --check       # what CI runs
```

## Conventions

- **Math conventions** follow `marks/mark-11-blueprint/chapters/00-notation.md`: twists are (ω, v) with angular first, and frames sit at link centres of mass.
- **Tests are the spec.** Every new test suite must be validated against a private reference solution (all tests pass) *and* against the stubs (zero pass), plus a few planted bugs (mutation checks). Never commit reference solutions.
- **Exit criteria are numbers.** Any new system needs measurable pass/fail thresholds.
- **References:** only cite works you have verified exist, with correct authors, year and venue. Add them to `tools/refs/catalog.py`, then regenerate.
- **Style:** the matrix/terminal aesthetic (`### \`> command\`` headings, ASCII diagrams generated or checked for alignment), GitHub-native math (fenced `math` blocks; `` $`…`$ `` when an inline expression contains `\\`).
- `CI` must stay green: `-Werror`, sanitizers, the COMPLETED regression gate, the reference freshness check, and the docs build.
