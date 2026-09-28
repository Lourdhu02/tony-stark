# The method

> Knowing the path and walking the path are different things. This page is about walking it.

A roadmap is a list of topics. What turns topics into skill is **the loop you run each week**. This is that loop, plus the rules that keep it honest.

## The loop

```text
            ┌────────────────────────────────────────────────────────┐
            │                                                        │
            ▼                                                        │
   ┌─────────────────┐   ┌─────────────────┐   ┌─────────────────┐   │
   │ 1. READ         │──▶│ 2. DERIVE       │──▶│ 3. BUILD        │   │
   │ one source,     │   │ on paper, before│   │ make the tests  │   │
   │ one chapter     │   │ touching code   │   │ pass, from zero │   │
   └─────────────────┘   └─────────────────┘   └────────┬────────┘   │
                                                        ▼            │
   ┌─────────────────┐   ┌─────────────────┐   ┌────────┴────────┐   │
   │ 6. REVIEW       │◀──│ 5. TEACH        │◀──│ 4. MEASURE      │   │
   │ spaced recall   │   │ lab note: your  │   │ benchmark, plot,│   │
   │ of old Marks    │   │ own words       │   │ vs. theory      │   │
   └────────┬────────┘   └─────────────────┘   └─────────────────┘   │
            └────────────────────────────────────────────────────────┘
```

| Step | What it looks like | Why it works |
|---|---|---|
| **1. Read** | One primary source per topic. The dossier names the chapter, so don't collect ten tutorials. | Depth over breadth. Switching sources feels productive but mostly re-reads the introduction. |
| **2. Derive** | Before coding, reproduce the key result on paper. Why is RK4 4th order? Why does coalescing need a footer? | If you can't derive it, you'll copy it. Copied code evaporates; derived code stays. |
| **3. Build** | Implement against the tests, stubs first and no libraries. | The tests are a spec written by someone else, just like a job. |
| **4. Measure** | Benchmarks, error plots, and numbers compared against theory. | Engineering is quantitative. "It works" is not a result; "7.6× at n=512" is. |
| **5. Teach** | Write the weekly lab note. Explain the hardest idea as if to your past self. | The Feynman test: gaps show up the moment you try to write the sentence. |
| **6. Review** | Every Sunday, 20 minutes: answer five random quiz questions from finished Marks without notes. | Retrieval practice beats re-reading, and the older Marks stay alive. |

## The weekly budget (about 11 hours)

```text
MON  ▓▓        theory · 2h     read + derive
TUE  ▓▓▓       build  · 3h     code against the tests
WED  ─         rest
THU  ▓▓        theory · 2h     read + derive
SAT  ▓▓▓       build  · 3h     code, measure, benchmark
SUN  ▓         write  · 1h     lab note + 20 min spaced review
```

Missing a session is fine; skipping the lab note is not. The note is how you know the week happened.

## Getting unstuck (in this order)

1. **Re-read the header.** Half of all bugs are a misread contract.
2. **Shrink the failing case.** Turn the failing test into a 5-line `main()` you can step through.
3. **Instrument.** Use `make SAN=1 test`, `gdb`, `valgrind` and `strace`. Watch what the machine actually did.
4. **Explain it out loud** (the rubber duck). Say what each line does. The bug is usually in the sentence you can't finish.
5. **Climb the hint ladder** in the project README: hint 1 is a nudge, hint 3 is the algorithm. Never code.
6. **Timebox:** 45 minutes stuck on the same symptom → take a break, write down what you know, and come back tomorrow. Sleep is a debugging tool.

The one rule: **never look up a solution's code.** Looking up a *concept* is research; looking up the *answer* robs you of the only part that builds skill.

## How to know you're improving

"I feel like I know more" is not evidence. These are:

| Signal | Measured by |
|---|---|
| Tests passing | `make test` and the armory dashboard |
| Speed of the second attempt | Re-implement a finished project from scratch a month later. Is it 3× faster? |
| Explanation quality | Could your lab note be someone else's tutorial? |
| Transfer | Do Mark V ideas show up unprompted when you think about Mark VII problems? |
| The rubric | Re-score yourself on the [self-assessment](self-assessment.md) at the end of every Mark |

## Anti-patterns

| Trap | Looks like | Instead |
|---|---|---|
| Tutorial hell | Watching the fourth video on Kalman filters | Stop at one source, derive, build |
| Collector's fallacy | 40 bookmarked papers, 0 read | The dossier's library is the whole list; everything else waits |
| Premature optimization of tools | A week configuring Neovim | Default editor, default flags. Build the thing. |
| Skipping the boring Mark | "I'll come back to electronics" | The tree has edges for a reason. Mark IV's timing work is what makes Mark V's controllers real. |
| Perfectionism | Rewriting the allocator a third time | Hit the exit criteria, write the note, move on. Stretch goals are optional. |
