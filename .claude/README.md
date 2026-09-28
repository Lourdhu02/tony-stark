# .claude/

Claude Code configuration for this repo. The lab ships with **six subagents**, each built around one rule: *the learner writes the solutions.*

| Agent | Use it when | Never does |
|---|---|---|
| [`tutor`](agents/tutor.md) | you're stuck on a test, a derivation or a concept | write your stubbed functions |
| [`reviewer`](agents/reviewer.md) | a system passes and you want a senior-engineer review | rewrite your code |
| [`quizmaster`](agents/quizmaster.md) | Sunday spaced review, or "quiz me" | show an answer before you try |
| [`paper-guide`](agents/paper-guide.md) | you start or get stuck in a paper from any `REFERENCES.md` | invent details it hasn't verified |
| [`lab-scribe`](agents/lab-scribe.md) | end of the week: draft the lab note | invent your learnings |
| [`curriculum-architect`](agents/curriculum-architect.md) | unlocking a new Mark or adding a system (maintainer) | commit reference solutions |

**Use them** from Claude Code in this repo, e.g. *"use the tutor agent, I'm stuck on `test_aba_inverts_rnea`"*. Repo-wide rules for any assistant live in [`../CLAUDE.md`](../CLAUDE.md).
