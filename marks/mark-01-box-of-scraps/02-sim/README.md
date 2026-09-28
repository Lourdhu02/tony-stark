# 02 · sim

> Week 2 · 13 tests · Field manual: [03 numerical integration](../notes/03-numerical-integration.md)

Three integrators and two physical models. Small code, big idea: **how you step time decides whether your physics is honest.**

### `> cat spec`

| Function | Contract | Tests |
|---|---|---|
| `pendulum_ode`, `pendulum_accel` | θ' = ω, ω' = −(g/L)·sin θ | `pendulum_ode_values` |
| `pendulum_energy` | ½L²ω² + gL(1 − cos θ) per unit mass | `pendulum_energy_values` |
| `spring_ode` | x' = v, v' = −(k/m)·x | `spring_ode_values` |
| `step_euler` | y ← y + dt·f(t, y) | `euler_is_first_order`, `euler_pumps_energy_in` |
| `step_rk4` | classic 4-stage RK | `rk4_is_fourth_order`, `rk4_matches_exact_spring`, `rk4_energy_drift_under_0_1_percent` |
| `step_verlet` | velocity Verlet: symplectic, 2nd order | `verlet_is_second_order`, `verlet_energy_stays_bounded`, `symplectic_beats_rk4_long_run` |
| (all of the above) | real physics | `small_angle_period`, `large_angle_period` |

### `> cat order_of_attack`

```text
models (5 min each) → euler → rk4 → verlet → make demo → plot phase portraits
```

### `> ./hints`

<details><summary><b>step_rk4</b> · hint 1</summary>

You need four slope arrays (k1…k4) *and* a temporary state array, because `f` must never see a half-updated `y`. `double k1[SIM_MAX_DIM]` on the stack is fine.
</details>

<details><summary><b>step_rk4</b> · hint 2</summary>

The time arguments matter even though these ODEs ignore t: k2 and k3 are evaluated at `t + dt/2`, and k4 at `t + dt`. Getting that wrong won't fail these tests, but it will fail you on time-varying systems in Mark V.
</details>

<details><summary><b>step_verlet</b> · hint</summary>

It's three lines of math across two calls to `a()`: a half-kick on v, a full drift on q, then a half-kick on v using the acceleration at the **new** q. If `verlet_is_second_order` reports a ratio near 2, you used the old acceleration twice, which quietly turns it into a 1st-order method.
</details>

<details><summary><b>order tests fail with ratio ≈ 1</b></summary>

The error isn't shrinking with dt. Either the stepper doesn't modify `y` in place, or the loop runs a fixed number of steps instead of `T/dt` steps.
</details>

### `> make demo && plot`

`make demo` writes `build/{euler,rk4,verlet}.csv` (columns `t, theta, omega, energy, drift`). Plot them with anything you like, for example:

```python
import pandas as pd, matplotlib.pyplot as plt
for m in ["euler", "rk4", "verlet"]:
    d = pd.read_csv(f"build/{m}.csv"); plt.plot(d.theta, d.omega, label=m, lw=0.6)
plt.xlabel("θ"); plt.ylabel("ω"); plt.legend(); plt.savefig("phase.png", dpi=150)
```

The picture you want: Euler spirals **out**, RK4 spirals **in**, and Verlet traces a **closed loop**. Commit the plot to your lab note.

### `> cat stretch.txt`

- Adaptive step size: Dormand–Prince RK45 with error control. How many steps does it take compared with fixed-step RK4 at the same accuracy?
- A double pendulum (chaotic!). Show how two runs with θ₀ differing by 10⁻⁹ diverge.
- Semi-implicit Euler, the default in most physics engines. Is it symplectic, and what order is it?
