import numpy as np
import pytest

from blueprint import so3

rng = np.random.default_rng(11)


def expm(A, terms=30):
    """Matrix exponential by scaling-and-squaring Taylor series: independent of your code."""
    s = max(0, int(np.ceil(np.log2(max(np.linalg.norm(A, 1), 1e-16)))) + 1)
    B = A / 2**s
    E, term = np.eye(len(A)), np.eye(len(A))
    for k in range(1, terms):
        term = term @ B / k
        E = E + term
    for _ in range(s):
        E = E @ E
    return E


def random_w(max_angle=np.pi - 1e-3):
    axis = rng.normal(size=3)
    return axis / np.linalg.norm(axis) * rng.uniform(0.0, max_angle)


def test_hat_is_cross_product():
    for _ in range(20):
        w, v = rng.normal(size=3), rng.normal(size=3)
        W = so3.hat(w)
        assert np.allclose(W, -W.T)
        assert np.allclose(W @ v, np.cross(w, v))
        assert np.allclose(so3.vee(W), w)


def test_exp_is_a_rotation():
    for _ in range(50):
        R = so3.exp(random_w())
        assert np.allclose(R.T @ R, np.eye(3), atol=1e-12)
        assert np.isclose(np.linalg.det(R), 1.0)


def test_exp_matches_matrix_exponential():
    for _ in range(20):
        w = random_w()
        assert np.allclose(so3.exp(w), expm(so3.hat(w)), atol=1e-12)


def test_exp_small_angle_is_stable():
    for scale in [1e-6, 1e-9, 1e-12, 0.0]:
        w = np.array([1.0, -2.0, 0.5]) * scale
        R = so3.exp(w)
        assert np.all(np.isfinite(R))
        assert np.allclose(R, np.eye(3) + so3.hat(w), atol=1e-12)


def test_log_inverts_exp():
    for _ in range(50):
        w = random_w()
        assert np.allclose(so3.log(so3.exp(w)), w, atol=1e-9)


def test_log_near_zero_and_near_pi():
    tiny = np.array([1e-10, -3e-10, 2e-10])
    assert np.allclose(so3.log(so3.exp(tiny)), tiny, atol=1e-14)
    for _ in range(10):
        axis = rng.normal(size=3)
        axis /= np.linalg.norm(axis)
        R = so3.exp(axis * (np.pi - 1e-9))
        w = so3.log(R)
        assert np.all(np.isfinite(w))
        assert np.allclose(so3.exp(w), R, atol=1e-6)  # at π, w and -w are the same rotation


def test_left_jacobian_first_order_property():
    """exp(w + δ) ≈ exp(J_l(w) δ) · exp(w), with an error of O(|δ|²)."""
    for _ in range(20):
        w = random_w(2.5)
        d = rng.normal(size=3) * 1e-6
        lhs = so3.exp(w + d)
        rhs = so3.exp(so3.left_jacobian(w) @ d) @ so3.exp(w)
        assert np.linalg.norm(lhs - rhs) < 1e-10


def test_left_jacobian_inverse():
    for w in [random_w(2.5) for _ in range(20)] + [np.zeros(3), np.array([1e-9, 0, 0])]:
        assert np.allclose(so3.left_jacobian_inv(w) @ so3.left_jacobian(w), np.eye(3), atol=1e-9)


def test_quaternion_round_trip_and_convention():
    s = np.sqrt(0.5)
    Rz90 = np.array([[0.0, -1, 0], [1, 0, 0], [0, 0, 1]])
    assert np.allclose(so3.quat_from_matrix(Rz90), [s, 0, 0, s])  # (w, x, y, z), w >= 0
    for _ in range(50):
        R = so3.exp(random_w())
        q = so3.quat_from_matrix(R)
        assert np.isclose(np.linalg.norm(q), 1.0)
        assert q[0] >= 0
        assert np.allclose(so3.matrix_from_quat(q), R, atol=1e-12)


@pytest.mark.parametrize("angle", [np.pi / 2, np.pi - 1e-3])
def test_quaternion_handles_negative_trace(angle):
    R = so3.exp(np.array([0.0, 1.0, 0.0]) * angle)
    assert np.allclose(so3.matrix_from_quat(so3.quat_from_matrix(R)), R, atol=1e-12)
