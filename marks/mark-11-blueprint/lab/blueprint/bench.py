"""bench.py: ABA vs CRBA + solve, as the chain grows (given code).

    cd marks/mark-11-blueprint/lab && python3 -m blueprint.bench
"""

import time

import numpy as np

from .chain import random_chain
from .dynamics import aba, mass_matrix, rnea


def _time(fn, reps=20):
    best = float("inf")
    for _ in range(reps):
        t0 = time.perf_counter()
        fn()
        best = min(best, time.perf_counter() - t0)
    return best


def main():
    rng = np.random.default_rng(0)
    print(f"  {'n':>4}  {'ABA (µs)':>10}  {'CRBA+solve (µs)':>16}  {'ratio':>6}")
    print("  ────  ──────────  ────────────────  ──────")
    for n in (5, 10, 20, 40, 80):
        ch = random_chain(n, seed=n)
        q, qd, tau = rng.normal(size=n), rng.normal(size=n), rng.normal(size=n)
        t_aba = _time(lambda: aba(ch, q, qd, tau))
        t_crba = _time(lambda: np.linalg.solve(mass_matrix(ch, q), tau - rnea(ch, q, qd, np.zeros(n))))
        print(f"  {n:>4}  {t_aba * 1e6:>10.0f}  {t_crba * 1e6:>16.0f}  {t_crba / t_aba:>6.2f}")


if __name__ == "__main__":
    main()
