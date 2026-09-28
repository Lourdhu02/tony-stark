"""se3.py: rigid motions SE(3) and twists se(3). Yours to implement.

Read chapters/02-rigid-motions.md first.
Twist ordering: ξ = (ω, v), angular part first. T is 4×4 homogeneous.
"""

import numpy as np

from . import so3


def make(R: np.ndarray, p: np.ndarray) -> np.ndarray:
    """Given: build T from R and p."""
    T = np.eye(4)
    T[:3, :3] = R
    T[:3, 3] = p
    return T


def hat(xi: np.ndarray) -> np.ndarray:
    """ξ = (ω, v) ∈ R⁶ → [ξ] ∈ se(3) (4×4, bottom row zero)."""
    raise NotImplementedError


def vee(X: np.ndarray) -> np.ndarray:
    """Inverse of hat."""
    raise NotImplementedError


def inverse(T: np.ndarray) -> np.ndarray:
    """T⁻¹ in closed form: (Rᵀ, -Rᵀ p). Don't call np.linalg.inv."""
    raise NotImplementedError


def exp(xi: np.ndarray) -> np.ndarray:
    """exp([ξ]) in closed form: rotation exp(ω), translation V(ω)·v.

    Chapter 02 §3 derives V(ω). Hint: you have already written it under another name.
    """
    raise NotImplementedError


def log(T: np.ndarray) -> np.ndarray:
    """Inverse of exp: returns ξ with |ω| ≤ π."""
    raise NotImplementedError


def adjoint(T: np.ndarray) -> np.ndarray:
    """6×6 [Ad_T]: maps a twist expressed in frame {b} to frame {a}, for T = T_ab."""
    raise NotImplementedError


def ad(xi: np.ndarray) -> np.ndarray:
    """6×6 [ad_ξ]: the Lie bracket as a matrix, [ad_a] b = vee([a][b] - [b][a])."""
    raise NotImplementedError
