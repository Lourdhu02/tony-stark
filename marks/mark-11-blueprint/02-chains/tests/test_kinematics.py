import numpy as np
import pytest

from blueprint import chain as models
from blueprint import se3
from blueprint.kinematics import body_jacobian, fk, link_twists

rng = np.random.default_rng(22)


def Ry(a):
    c, s = np.cos(a), np.sin(a)
    return np.array([[c, 0, s], [0, 1, 0], [-s, 0, c]])


def test_fk_pendulum():
    p = models.pendulum(l=2.0)
    for q in [0.0, 0.4, -1.2, np.pi]:
        T = fk(p, [q])[0]
        assert np.allclose(T[:3, 3], [-2.0 * np.sin(q), 0, -2.0 * np.cos(q)], atol=1e-12)


def test_fk_double_pendulum_matches_geometry():
    l1, l2 = 0.7, 1.3
    dp = models.double_pendulum(l1=l1, l2=l2)
    for _ in range(20):
        q1, q2 = rng.uniform(-np.pi, np.pi, size=2)
        T1, T2 = fk(dp, [q1, q2])
        p1 = np.array([-l1 * np.sin(q1), 0, -l1 * np.cos(q1)])
        p2 = p1 + [-l2 * np.sin(q1 + q2), 0, -l2 * np.cos(q1 + q2)]
        assert np.allclose(T1[:3, 3], p1, atol=1e-12)
        assert np.allclose(T2[:3, 3], p2, atol=1e-12)
        assert np.allclose(T2[:3, :3], Ry(q1 + q2), atol=1e-12)


def test_fk_home_is_product_of_M():
    ch = models.random_chain(5, seed=1)
    Ts = fk(ch, np.zeros(5))
    P = np.eye(4)
    for i in range(5):
        P = P @ ch.M[i]
        assert np.allclose(Ts[i], P, atol=1e-12)


@pytest.mark.parametrize("seed", [0, 1, 2])
def test_link_twists_match_finite_differences(seed):
    """[V_i] = T_{0i}⁻¹ · dT_{0i}/dt: the body twist is the pose's velocity, seen from the body."""
    ch = models.random_chain(6, seed=seed, prismatic=(2,))
    q, qd = rng.normal(size=6), rng.normal(size=6)
    eps = 1e-6
    Tp, Tm, T0 = fk(ch, q + eps * qd), fk(ch, q - eps * qd), fk(ch, q)
    for i, V in enumerate(link_twists(ch, q, qd)):
        Tdot = (Tp[i] - Tm[i]) / (2 * eps)
        assert np.allclose(se3.hat(V), se3.inverse(T0[i]) @ Tdot, atol=1e-6)


def test_body_jacobian_maps_joint_rates_to_tip_twist():
    ch = models.random_chain(6, seed=4, prismatic=(1,))
    for _ in range(10):
        q, qd = rng.normal(size=6), rng.normal(size=6)
        J = body_jacobian(ch, q)
        assert J.shape == (6, 6)
        assert np.allclose(J @ qd, link_twists(ch, q, qd)[-1], atol=1e-10)


def test_body_jacobian_last_column_is_last_axis():
    ch = models.random_chain(4, seed=5)
    J = body_jacobian(ch, rng.normal(size=4))
    assert np.allclose(J[:, -1], ch.A[-1])
