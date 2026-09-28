"""kinematics.py: forward kinematics and velocity propagation along a chain. Yours.

Read chapters/03-kinematic-chains.md first. Model conventions are in chain.py.
"""

import numpy as np

from . import se3


def fk(chain, q) -> list:
    """Poses T_{0,i} of every link frame, i = 1..n, as a list of 4×4 arrays.

    T_{0,i} = T_{0,i-1} · M_{i-1,i} · exp([A_i] q_i)
    """
    raise NotImplementedError


def link_twists(chain, q, qd) -> list:
    """Body twists V_i of every link, each expressed in its own frame {i}.

    V_i = [Ad_{T_{i,i-1}}] V_{i-1} + A_i q̇_i,   with V_0 = 0
    where T_{i,i-1} = exp(-[A_i] q_i) · M_{i-1,i}⁻¹.
    """
    raise NotImplementedError


def body_jacobian(chain, q) -> np.ndarray:
    """6×n J_b(q) with V_n = J_b q̇ (tip twist in the last link's frame).

    Column i is joint i's screw axis carried into frame {n}: [Ad_{T_{n,i}}] A_i.
    """
    raise NotImplementedError
