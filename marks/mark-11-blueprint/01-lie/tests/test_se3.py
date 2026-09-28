import numpy as np

from blueprint import se3, so3
from test_so3 import expm

rng = np.random.default_rng(3)


def random_xi(max_angle=np.pi - 1e-3):
    axis = rng.normal(size=3)
    w = axis / np.linalg.norm(axis) * rng.uniform(0.0, max_angle)
    return np.concatenate([w, rng.normal(size=3)])


def random_T():
    return se3.exp(random_xi())


def test_hat_vee_round_trip():
    for _ in range(20):
        xi = rng.normal(size=6)
        X = se3.hat(xi)
        assert X.shape == (4, 4) and np.allclose(X[3], 0)
        assert np.allclose(se3.vee(X), xi)


def test_exp_matches_matrix_exponential():
    for _ in range(20):
        xi = random_xi()
        assert np.allclose(se3.exp(xi), expm(se3.hat(xi)), atol=1e-12)


def test_pure_translation():
    T = se3.exp(np.array([0, 0, 0, 1.0, -2.0, 0.5]))
    assert np.allclose(T[:3, :3], np.eye(3))
    assert np.allclose(T[:3, 3], [1.0, -2.0, 0.5])


def test_log_inverts_exp():
    for _ in range(50):
        xi = random_xi()
        assert np.allclose(se3.log(se3.exp(xi)), xi, atol=1e-9)


def test_inverse():
    for _ in range(20):
        T = random_T()
        assert np.allclose(T @ se3.inverse(T), np.eye(4), atol=1e-12)
        assert np.allclose(se3.inverse(T), np.linalg.inv(T), atol=1e-12)


def test_adjoint_conjugation():
    """T · exp([ξ]) · T⁻¹ = exp([Ad_T ξ]): the adjoint changes the frame of a twist."""
    for _ in range(20):
        T, xi = random_T(), random_xi(1.0)
        assert np.allclose(T @ se3.exp(xi) @ se3.inverse(T), se3.exp(se3.adjoint(T) @ xi), atol=1e-10)


def test_adjoint_is_a_homomorphism():
    for _ in range(20):
        T1, T2 = random_T(), random_T()
        assert np.allclose(se3.adjoint(T1 @ T2), se3.adjoint(T1) @ se3.adjoint(T2), atol=1e-10)


def test_small_adjoint_is_the_lie_bracket():
    for _ in range(20):
        a, b = rng.normal(size=6), rng.normal(size=6)
        A, B = se3.hat(a), se3.hat(b)
        assert np.allclose(se3.hat(se3.ad(a) @ b), A @ B - B @ A, atol=1e-12)


def test_screw_motion_fixes_its_axis():
    """A pure rotation about z through (1, 0, 0) leaves that point where it is."""
    xi = np.concatenate([[0, 0, 1.0], -np.cross([0, 0, 1.0], [1.0, 0, 0])])
    for theta in [0.3, 1.7, -2.5]:
        T = se3.exp(xi * theta)
        assert np.allclose(T @ [1, 0, 0, 1], [1, 0, 0, 1], atol=1e-12)
        origin = T @ [0, 0, 0, 1]
        assert np.isclose(np.linalg.norm(origin[:3] - [1, 0, 0]), 1.0)
        assert np.allclose(T[:3, :3], so3.exp(np.array([0, 0, theta])), atol=1e-12)
