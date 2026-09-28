# Field manual 03 · Numerical integration

> Used by: `02-sim` · Later: Mark V (quadrotor simulator), Mark VI (MuJoCo), Mark X (sim-to-real)

**The one idea:** a simulator is `state ← state + (how it changes) × dt`, done carefully. *How* carefully decides whether your physics conserves energy or quietly invents it.

## 1. The problem

Every physical system in this lab is an ordinary differential equation:

```text
y' = f(t, y)          e.g. pendulum:  y = [θ, ω],   f = [ω, −(g/L)·sin θ]
```

There's no closed-form solution for the pendulum at large angles, so we **step**: from yₙ at time tₙ, estimate yₙ₊₁ at tₙ + h.

## 2. Euler, from Taylor

```text
y(t + h) = y(t) + h·y'(t) + (h²/2)·y''(t) + O(h³)
         └──────────────┘   └──────────────────┘
          explicit Euler        what Euler drops
```

- **Local error** (one step): O(h²).
- **Global error** (after T/h steps): O(h²) × (T/h) = **O(h)**, so Euler is 1st order.

A method of **order p** has global error ∝ hᵖ. Halve h and the error drops by 2ᵖ. That's exactly what the tests check:

| Method | Order p | Halve dt → error ÷ | Test |
|---|---|---|---|
| Euler | 1 | 2 | `euler_is_first_order` |
| Velocity Verlet | 2 | 4 | `verlet_is_second_order` |
| RK4 | 4 | 16 | `rk4_is_fourth_order` |

## 3. RK4: sample the slope four times

```text
k1 = f(t,       y)              slope at the start
k2 = f(t + h/2, y + h/2·k1)     slope at the midpoint, using k1
k3 = f(t + h/2, y + h/2·k2)     slope at the midpoint, using k2
k4 = f(t + h,   y + h·k3)       slope at the end
y ← y + h/6 · (k1 + 2k2 + 2k3 + k4)
```

The weights 1-2-2-1 are Simpson's rule in disguise. The four evaluations are chosen so that the Taylor terms match through h⁴. For the cost of 4 function evaluations per step, RK4 is 3 orders more accurate than Euler.

## 4. Stability: why Euler spirals out

Apply Euler to the test equation y' = λy:

```text
yₙ₊₁ = (1 + hλ) · yₙ     →  stable only if |1 + hλ| ≤ 1
```

An undamped oscillator has **imaginary** λ = ±iω, so

```text
|1 + ihω| = √(1 + h²ω²)  >  1     for every h > 0
```

Explicit Euler **amplifies every oscillation, for every step size.** The energy grows geometrically. That's the `euler_pumps_energy_in` test: +113% in 10 s at dt = 0.01.

## 5. Symplectic integrators: respecting the geometry

A frictionless pendulum is a **Hamiltonian system**: energy H(q, p) = kinetic + potential is conserved, and the flow preserves **phase-space area** (Liouville's theorem).

- **RK4** is accurate per step but *not* area-preserving. Its tiny per-step errors all push in the same direction, so energy **drifts** (for the pendulum, it slowly bleeds away).
- **Velocity Verlet** is *symplectic*: it exactly preserves the area of a slightly perturbed "shadow" Hamiltonian. The energy error **oscillates but never accumulates**.

```text
v_half = v + a(q)·h/2
q      = q + v_half·h
v      = v_half + a(q)·h/2
```

Measured in this repo (pendulum, θ₀ = 1 rad, **dt = 0.1**, **10,000 s** = 10⁵ steps):

| Method | Order | Max \|ΔE/E₀\| | Behaviour |
|---|---|---|---|
| Euler | 1 | +11,790% | runs away |
| RK4 | 4 | −64.6% | slow, steady drain |
| Verlet | 2 | 2.3% | bounded forever |

**A 2nd-order method beats a 4th-order one** over long horizons because it preserves the right *structure*. Order is not everything.

## 6. Where you'll meet this again

- **Physics engines** (MuJoCo, Bullet, most game engines) default to *semi-implicit* ("symplectic") Euler, which is the 1st-order cousin of Verlet, for exactly this stability.
- **Mark V:** your quadrotor simulator. Rotations add a twist: integrating quaternions requires renormalizing or using the exponential map.
- **Mark X:** sim-to-real gaps often *are* integrator artifacts (energy that the real robot doesn't have).

## Exercises

1. Plot phase portraits (θ vs ω) for all three methods at dt = 0.1 using `make demo`. Euler spirals out, RK4 spirals in, and Verlet traces a closed loop.
2. Implement **semi-implicit Euler** (update v first, then q using the *new* v). Show that it's 1st order but its energy is bounded.
3. Find the largest dt at which Verlet stays stable for the spring with ω = 2. (Hint: it's related to hω < 2.)
4. Add damping (friction) to the pendulum. Does the symplectic advantage survive? Why or why not?
