# Security

This repository is **educational**. The systems built here (`malloc`, a shell, a kernel, firmware, an LLM agent) are learning implementations and **are not hardened for production use**. Don't deploy them where security matters.

## Reporting

If you find a security issue in the repository's **infrastructure** (for example the CI workflows, the site build, or a script that could harm someone who runs it), report it privately with [GitHub's private vulnerability reporting](https://github.com/Lourdhu02/tony-stark/security/advisories/new) rather than in a public issue. You can expect an acknowledgement within a week.

## Safety beyond software

Marks IV–X involve real hardware: batteries, motors and robot arms. Read the safety sections in [`HARDWARE.md`](HARDWARE.md) and the [Mark IV dossier](marks/mark-04-arc-reactor/README.md). Mark IX connects an LLM to actuators; its dossier specifies a deterministic safety layer that must sit outside the model.
