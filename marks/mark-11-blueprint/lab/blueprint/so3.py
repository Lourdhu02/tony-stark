"""so3.py: the rotation group SO(3) and its Lie algebra so(3). Yours to implement.

Read chapters/01-rotations.md first. Every function here has a derivation there.
Vectors are numpy arrays of shape (3,), matrices (3, 3).
"""

import numpy as np


def hat(w: np.ndarray) -> np.ndarray:
    """w ∈ R³ → [w] ∈ so(3), the skew-symmetric matrix with [w] v = w × v."""
    raise NotImplementedError


def vee(W: np.ndarray) -> np.ndarray:
    """Inverse of hat: so(3) → R³."""
    raise NotImplementedError


def exp(w: np.ndarray) -> np.ndarray:
    """Rodrigues' formula: rotation by angle |w| about axis w/|w|.

    Must be accurate and finite for |w| → 0 (use the Taylor expansion below ~1e-8).
    """
    raise NotImplementedError


def log(R: np.ndarray) -> np.ndarray:
    """Inverse of exp for angles in [0, π]. Returns w with |w| ≤ π.

    Three regimes: θ ≈ 0 (first-order), generic, and θ ≈ π, where sin θ → 0
    and the generic formula divides by zero. Chapter 01 §5 derives the fix.
    """
    raise NotImplementedError


def left_jacobian(w: np.ndarray) -> np.ndarray:
    """J_l(w): the 3×3 matrix with exp(w + δ) ≈ exp(J_l(w) δ) · exp(w) for small δ."""
    raise NotImplementedError


def left_jacobian_inv(w: np.ndarray) -> np.ndarray:
    """Closed-form inverse of J_l(w) (don't call np.linalg.inv)."""
    raise NotImplementedError


def quat_from_matrix(R: np.ndarray) -> np.ndarray:
    """Unit quaternion (w, x, y, z), Hamilton convention, with w ≥ 0.

    Needs a numerically safe branch when trace(R) ≤ 0 (Shepperd's method).
    """
    raise NotImplementedError


def matrix_from_quat(q: np.ndarray) -> np.ndarray:
    """Rotation matrix from a quaternion (w, x, y, z); normalize the input first."""
    raise NotImplementedError
