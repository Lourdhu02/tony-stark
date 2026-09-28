# Field manuals

Read the manual **before** starting the project it serves. Each one covers the theory the tests assume you know, with the derivations and diagrams, and ends with exercises that go beyond the tests.

| # | Manual | Serves | Core idea |
|---|---|---|---|
| 01 | [The memory hierarchy](01-memory-hierarchy.md) | `01-linalg` | Loop order and tiling decide speed, not arithmetic |
| 02 | [LU and floating point](02-lu-and-floating-point.md) | `01-linalg` | Elimination is factorization; pivoting keeps rounding error in check |
| 03 | [Numerical integration](03-numerical-integration.md) | `02-sim` | Order of accuracy vs preserving structure (symplectic methods) |
| 04 | [Memory allocators](04-allocators.md) | `03-malloc` | Boundary tags, free lists, coalescing, interposition |
| 05 | [Processes, pipes and signals](05-processes-pipes-signals.md) | `04-shell` | fork + fd surgery + exec, the EOF rule and signal inheritance |
