import numpy as np
import pytest

from blueprint import chain as models
from blueprint.dynamics import aba, energy, mass_matrix, rnea
from blueprint.sim import simulate

rng = np.random.default_rng(33)
g = 9.81


def double_pendulum_lagrangian(q, qd, m1, m2, l1, l2):
    """Closed-form M(q), Coriolis/centrifugal h(q, q̇) and gravity g(q), from the textbook Lagrangian."""
    q1, q2 = q
    d1, d2 = qd
    c2, s2 = np.cos(q2), np.sin(q2)
    M = np.array([
        [m1 * l1**2 + m2 * (l1**2 + l2**2 + 2 * l1 * l2 * c2), m2 * (l2**2 + l1 * l2 * c2)],
        [m2 * (l2**2 + l1 * l2 * c2), m2 * l2**2],
    ])
    h = np.array([-m2 * l1 * l2 * s2 * (2 * d1 * d2 + d2**2), m2 * l1 * l2 * s2 * d1**2])
    grav = np.array([
        (m1 + m2) * g * l1 * np.sin(q1) + m2 * g * l2 * np.sin(q1 + q2),
        m2 * g * l2 * np.sin(q1 + q2),
    ])
    return M, h, grav


# ── inverse dynamics ────────────────────────────────────────────────────


def test_rnea_pendulum_gravity_torque():
    p = models.pendulum(m=2.0, l=0.5)
    for q in [0.0, 0.3, 1.2, -2.0]:
        assert np.isclose(rnea(p, [q], [0.0], [0.0])[0], 2.0 * g * 0.5 * np.sin(q))


def test_rnea_matches_double_pendulum_lagrangian():
    m1, m2, l1, l2 = 1.3, 0.7, 0.9, 1.1
    dp = models.double_pendulum(m1, m2, l1, l2)
    for _ in range(25):
        q, qd, qdd = rng.normal(size=2), rng.normal(size=2), rng.normal(size=2)
        M, h, grav = double_pendulum_lagrangian(q, qd, m1, m2, l1, l2)
        assert np.allclose(rnea(dp, q, qd, qdd), M @ qdd + h + grav, atol=1e-10)


def test_rnea_without_gravity_at_rest_is_zero():
    ch = models.random_chain(6, seed=7)
    assert np.allclose(rnea(ch, rng.normal(size=6), np.zeros(6), np.zeros(6), gravity=False), 0)


# ── the mass matrix ─────────────────────────────────────────────────────


def test_mass_matrix_matches_double_pendulum():
    m1, m2, l1, l2 = 1.3, 0.7, 0.9, 1.1
    dp = models.double_pendulum(m1, m2, l1, l2)
    for _ in range(10):
        q = rng.normal(size=2)
        assert np.allclose(mass_matrix(dp, q), double_pendulum_lagrangian(q, [0, 0], m1, m2, l1, l2)[0], atol=1e-12)


def test_mass_matrix_is_symmetric_positive_definite():
    ch = models.random_chain(7, seed=8, prismatic=(3,))
    for _ in range(10):
        M = mass_matrix(ch, rng.normal(size=7))
        assert np.allclose(M, M.T, atol=1e-12)
        assert np.all(np.linalg.eigvalsh(M) > 0)


def test_crba_equals_rnea_columns():
    """Column j of M is the torque needed for q̈ = e_j, with q̇ = 0 and no gravity."""
    ch = models.random_chain(6, seed=9, prismatic=(0, 4))
    q = rng.normal(size=6)
    cols = [rnea(ch, q, np.zeros(6), e, gravity=False) for e in np.eye(6)]
    assert np.allclose(mass_matrix(ch, q), np.column_stack(cols), atol=1e-10)


def test_passivity_identity():
    """q̇ᵀ Ṁ q̇ = 2 q̇ᵀ C(q, q̇) q̇: the energy identity behind every passivity-based controller."""
    ch = models.random_chain(6, seed=10)
    for _ in range(5):
        q, qd = rng.normal(size=6), rng.normal(size=6)
        eps = 1e-6
        Mdot = (mass_matrix(ch, q + eps * qd) - mass_matrix(ch, q - eps * qd)) / (2 * eps)
        Cqd = rnea(ch, q, qd, np.zeros(6), gravity=False)
        assert np.isclose(qd @ Mdot @ qd, 2 * qd @ Cqd, rtol=1e-5, atol=1e-8)


# ── forward dynamics ────────────────────────────────────────────────────


@pytest.mark.parametrize("seed", [11, 12, 13])
def test_aba_inverts_rnea(seed):
    ch = models.random_chain(7, seed=seed, prismatic=(2,))
    for _ in range(5):
        q, qd, qdd = rng.normal(size=7), rng.normal(size=7), rng.normal(size=7)
        assert np.allclose(aba(ch, q, qd, rnea(ch, q, qd, qdd)), qdd, atol=1e-9)


def test_aba_matches_mass_matrix_solve():
    ch = models.random_chain(6, seed=14)
    q, qd, tau = rng.normal(size=6), rng.normal(size=6), rng.normal(size=6)
    bias = rnea(ch, q, qd, np.zeros(6))  # h(q, q̇) + g(q)
    assert np.allclose(aba(ch, q, qd, tau), np.linalg.solve(mass_matrix(ch, q), tau - bias), atol=1e-9)


def test_free_fall_on_a_vertical_slider():
    ch = models.Chain(M=[np.eye(4)], A=[models.prismatic_axis([0, 0, 1])],
                      G=[models.spatial_inertia(3.0, np.eye(3) * 0.01)])
    assert np.isclose(aba(ch, [0.0], [0.0], [0.0])[0], -g)
    assert np.isclose(aba(ch, [0.0], [0.0], [3.0 * g])[0], 0.0)  # thrust exactly cancels gravity


# ── energy ──────────────────────────────────────────────────────────────


def test_energy_of_a_pendulum():
    p = models.pendulum(m=2.0, l=0.5)
    assert np.isclose(energy(p, [0.0], [0.0]), 2.0 * g * -0.5)          # potential only, mass at z = -l
    assert np.isclose(energy(p, [0.0], [3.0]), 0.5 * 2.0 * (0.5 * 3.0) ** 2 + 2.0 * g * -0.5)


def test_energy_is_conserved_in_simulation():
    ch = models.random_chain(6, seed=15, prismatic=(3,))
    q0, qd0 = rng.normal(size=6) * 0.5, rng.normal(size=6) * 0.5
    t, q, qd = simulate(ch, q0, qd0, dt=1e-3, T=1.0)
    E = np.array([energy(ch, q[k], qd[k]) for k in range(0, len(t), 100)])
    assert np.max(np.abs(E - E[0])) / (abs(E[0]) + 1.0) < 1e-6
