"""sim.py: integrate a chain forward in time (given code).

Uses RK4 from Mark I, on the state (q, q̇), with your forward dynamics.
"""

import numpy as np


def simulate(chain, q0, qd0, dt: float, T: float, tau=None, forward_dynamics=None):
    """Return arrays (t, q, qd) sampled every step.

    tau:               callable (t, q, qd) -> joint torques, or None for zero torque
    forward_dynamics:  callable (chain, q, qd, tau) -> qdd; defaults to your `aba`
    """
    if forward_dynamics is None:
        from .dynamics import aba as forward_dynamics

    n = chain.n
    tau = tau or (lambda t, q, qd: np.zeros(n))

    def f(t, x):
        q, qd = x[:n], x[n:]
        return np.concatenate([qd, forward_dynamics(chain, q, qd, tau(t, q, qd))])

    steps = int(round(T / dt))
    x = np.concatenate([np.asarray(q0, float), np.asarray(qd0, float)])
    ts, xs = [0.0], [x.copy()]
    for k in range(steps):
        t = k * dt
        k1 = f(t, x)
        k2 = f(t + dt / 2, x + dt / 2 * k1)
        k3 = f(t + dt / 2, x + dt / 2 * k2)
        k4 = f(t + dt, x + dt * k3)
        x = x + dt / 6 * (k1 + 2 * k2 + 2 * k3 + k4)
        ts.append(t + dt)
        xs.append(x.copy())
    xs = np.array(xs)
    return np.array(ts), xs[:, :n], xs[:, n:]
