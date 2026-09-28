# Mark XI · Chapters

A short graduate textbook on rigid-body geometry and dynamics, written to be read **before** the code it serves. Each chapter builds intuition, derives every result step by step, gives the algorithm, explains what the tests check, and ends with exercises (★ = do twice).

| # | Chapter | Core result | Lab |
|---|---|---|---|
| 00 | [Notation and conventions](00-notation.md) | twist $(\omega, v)$, wrench $(m, f)$, and a Rosetta stone for other libraries | — |
| 01 | [Rotations done right: SO(3)](01-rotations.md) | Rodrigues, the three-regime log, quaternions, the left Jacobian | `so3.py` |
| 02 | [Rigid motions: SE(3), twists, wrenches](02-rigid-motions.md) | $e^{[\xi]} = (\exp\omega,\ J_l(\omega)v)$, adjoint, bracket, wrench duality | `se3.py` |
| 03 | [Kinematic chains](03-kinematic-chains.md) | O(n) velocity and acceleration propagation; the body Jacobian | `kinematics.py` |
| 04 | [One rigid body: Newton–Euler](04-newton-euler.md) | $\mathcal F = \mathcal G\dot{\mathcal V} - \mathrm{ad}^\top_{\mathcal V}\mathcal G\mathcal V$; spatial inertia; physical consistency | `dynamics.py` |
| 05 | [Inverse dynamics: RNEA](05-rnea.md) | two sweeps, O(n); the gravity trick, proved | `dynamics.rnea` |
| 06 | [The mass matrix, CRBA, passivity](06-mass-matrix.md) | composite inertias; $\dot M - 2C$ skew; PD + gravity compensation is stable | `dynamics.mass_matrix` |
| 07 | [Forward dynamics: ABA](07-aba.md) | articulated-body inertia by induction; O(n) forward dynamics | `dynamics.aba` |
| 08 | [Constraints and contact](08-contact.md) | KKT, Gauss's principle, complementarity, how simulators differ | stretch |

**Reading order:** 00 → 01 → 02 → 03 → 04 → 05 → 06 → 07 → 08. Chapters 04–07 depend on each other tightly. Don't skip 04.
