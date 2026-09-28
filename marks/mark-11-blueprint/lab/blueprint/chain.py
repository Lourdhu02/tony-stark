"""chain.py: serial kinematic chains (given code).

Conventions (full details in chapters/00-notation.md):

    twist   V = (ω, v) ∈ R⁶, angular part first (Lynch & Park; Featherstone).
            Pinocchio orders twists (v, ω). Mind the swap when cross-checking.
    wrench  F = (m, f) ∈ R⁶, moment first. Power = Fᵀ V.

    Frame {0} is the space (world) frame. Frame {i} is rigidly attached to
    link i **at its centre of mass**, which makes every spatial inertia
    block-diagonal: G_i = diag(I_c, m·1).

A Chain with n joints stores, for joint/link i = 1..n (list index i-1):

    M[i-1]  M_{i-1,i}: pose of {i} relative to {i-1} when all joints are zero (4×4)
    A[i-1]  A_i: screw axis of joint i, expressed in frame {i}           (6,)
    G[i-1]  G_i: spatial inertia of link i, expressed in frame {i}        (6×6)

Joint i moves link i relative to link i-1:

    T_{i-1,i}(q_i) = M_{i-1,i} · exp([A_i] q_i)
"""

from dataclasses import dataclass, field

import numpy as np

GRAVITY = np.array([0.0, 0.0, -9.81])


@dataclass
class Chain:
    M: list
    A: list
    G: list
    g: np.ndarray = field(default_factory=lambda: GRAVITY.copy())

    @property
    def n(self) -> int:
        return len(self.A)

    def mass(self, i: int) -> float:
        """Mass of link i+1 (0-based list index)."""
        return float(self.G[i][3, 3])


# ── building blocks ─────────────────────────────────────────────────────


def translation(x: float, y: float, z: float) -> np.ndarray:
    T = np.eye(4)
    T[:3, 3] = (x, y, z)
    return T


def revolute_axis(omega, point) -> np.ndarray:
    """Screw axis of a revolute joint: unit direction ω through `point` (both in {i})."""
    w = np.asarray(omega, dtype=float)
    w = w / np.linalg.norm(w)
    return np.concatenate([w, -np.cross(w, np.asarray(point, dtype=float))])


def prismatic_axis(direction) -> np.ndarray:
    """Screw axis of a prismatic joint sliding along `direction` (in {i})."""
    d = np.asarray(direction, dtype=float)
    return np.concatenate([np.zeros(3), d / np.linalg.norm(d)])


def spatial_inertia(mass: float, I_c) -> np.ndarray:
    """G = diag(I_c, m·1) for a frame at the centre of mass."""
    G = np.zeros((6, 6))
    G[:3, :3] = I_c
    G[3:, 3:] = mass * np.eye(3)
    return G


# ── example robots ──────────────────────────────────────────────────────


def pendulum(m: float = 1.0, l: float = 1.0) -> Chain:
    """A point mass m on a massless rod of length l, hinged at the origin.

    The joint rotates about +y. At q = 0 the mass hangs straight down at (0, 0, -l);
    positive q swings it towards -x:  p(q) = (-l·sin q, 0, -l·cos q).
    """
    return Chain(
        M=[translation(0, 0, -l)],
        A=[revolute_axis([0, 1, 0], [0, 0, l])],
        G=[spatial_inertia(m, np.zeros((3, 3)))],
    )


def double_pendulum(m1=1.0, m2=1.0, l1=1.0, l2=1.0) -> Chain:
    """Two point masses on massless rods; q2 is measured *relative to* link 1.

        p1 = (-l1·sin q1, 0, -l1·cos q1)
        p2 = p1 + (-l2·sin(q1+q2), 0, -l2·cos(q1+q2))
    """
    return Chain(
        M=[translation(0, 0, -l1), translation(0, 0, -l2)],
        A=[revolute_axis([0, 1, 0], [0, 0, l1]), revolute_axis([0, 1, 0], [0, 0, l2])],
        G=[spatial_inertia(m1, np.zeros((3, 3))), spatial_inertia(m2, np.zeros((3, 3)))],
    )


def _random_rotation(rng) -> np.ndarray:
    Q, R = np.linalg.qr(rng.normal(size=(3, 3)))
    Q = Q @ np.diag(np.sign(np.diag(R)))
    if np.linalg.det(Q) < 0:
        Q[:, 0] = -Q[:, 0]
    return Q


def random_chain(n: int = 6, seed: int = 0, prismatic=()) -> Chain:
    """A random, physically valid n-joint chain with full 3D rigid-body links.

    Joint indices listed in `prismatic` (0-based) are prismatic; the rest are revolute.
    Link inertias satisfy the triangle inequality (they are real rigid bodies).
    """
    rng = np.random.default_rng(seed)
    M, A, G = [], [], []
    for i in range(n):
        T = np.eye(4)
        T[:3, :3] = _random_rotation(rng)
        T[:3, 3] = rng.uniform(-0.3, 0.3, size=3)
        M.append(T)

        if i in prismatic:
            A.append(prismatic_axis(rng.normal(size=3)))
        else:
            A.append(revolute_axis(rng.normal(size=3), rng.uniform(-0.2, 0.2, size=3)))

        x, y, z = rng.uniform(0.01, 0.1, size=3)
        R = _random_rotation(rng)
        I_c = R @ np.diag([y + z, x + z, x + y]) @ R.T  # principal moments obey the triangle inequality
        G.append(spatial_inertia(rng.uniform(0.5, 3.0), I_c))
    return Chain(M=M, A=A, G=G)
