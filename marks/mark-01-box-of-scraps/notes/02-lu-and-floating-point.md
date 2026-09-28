# Field manual 02 · LU decomposition and floating point

> Used by: `01-linalg` (`mat_lu`, `mat_solve`, `mat_inverse`, `mat_det`) · Later: Mark V (LQR), Mark VII (every estimator solves linear systems)

**The one idea:** Gaussian elimination *is* a factorization, A = LU. **Pivoting** is what keeps the rounding errors of finite-precision arithmetic from destroying the answer.

## 1. Floating point in five facts

A `double` (IEEE 754 binary64) has 1 sign bit, 11 exponent bits and 52 fraction bits.

| Fact | Consequence |
|---|---|
| Machine epsilon ε = 2⁻⁵² ≈ 2.2 × 10⁻¹⁶ | About 16 significant decimal digits, and never more |
| Most decimals aren't representable | `0.1 + 0.2 != 0.3`. Compare with a tolerance. |
| Every operation rounds | `fl(a ∘ b) = (a ∘ b)(1 + δ)`, with \|δ\| ≤ ε/2 |
| Addition isn't associative | `(a + b) + c ≠ a + (b + c)`, so summation order changes results |
| Subtracting nearly-equal numbers cancels digits | `1.0000001 − 1.0` keeps only the digits that differ, and the error stays |

## 2. Elimination = factorization

Eliminating below the first pivot of a 3×3 matrix means subtracting multiples of row 0:

```text
      ┌             ┐            multipliers  l₁₀ = a₁₀/a₀₀,  l₂₀ = a₂₀/a₀₀
A  =  │ a₀₀ a₀₁ a₀₂ │
      │ a₁₀ a₁₁ a₁₂ │   row₁ ← row₁ − l₁₀·row₀
      │ a₂₀ a₂₁ a₂₂ │   row₂ ← row₂ − l₂₀·row₀
      └             ┘
```

Record every multiplier in L and keep the reduced matrix as U:

```text
      ┌             ┐   ┌             ┐
L  =  │  1   0   0  │   │ u₀₀ u₀₁ u₀₂ │  =  U
      │ l₁₀  1   0  │   │  0  u₁₁ u₁₂ │
      │ l₂₀ l₂₁  1  │   │  0   0  u₂₂ │
      └             ┘   └             ┘
```

**Cost:** about ⅔n³ FLOPs to factor, then about 2n² per right-hand side (a forward solve with L, then a back solve with U).
That's why you **factor once and solve many times**, and why you should **never compute A⁻¹ just to solve Ax = b**: it costs roughly 3× more and is less accurate.

## 3. Why pivoting isn't optional

Take ε = 10⁻²⁰ (tiny, but not zero):

```text
A = │ ε  1 │    b = │ 1 │    true solution x ≈ (1, 1)
    │ 1  1 │        │ 2 │
```

**Without pivoting:** l₁₀ = 1/ε = 10²⁰, so u₁₁ = 1 − 10²⁰, which rounds to −10²⁰. The "1" has vanished.
Back-substitution then gives x₁ = 1 and x₀ = (1 − x₁)/ε = **0**. The answer is completely wrong.

**With partial pivoting:** swap the rows first, because |1| > |ε|. The multiplier becomes ε, which is tiny, and the result is x ≈ (1, 1). Correct.

The rule: at step k, pick the row with the **largest |aᵢₖ|** at or below the diagonal. Then every multiplier has |lᵢₖ| ≤ 1, and errors can't be amplified by huge multipliers. Your `perm` array records the swaps, so P·A = L·U.

## 4. The determinant comes for free

```text
det(A) = det(P)⁻¹ · det(L) · det(U) = (−1)^(number of swaps) · Π uᵢᵢ
```

Never use cofactor expansion: it's O(n!).

## 5. Conditioning: when even perfect code gives bad answers

The **condition number** κ(A) = ‖A‖·‖A⁻¹‖ measures how much A amplifies relative errors:

```text
relative error in x  ≲  κ(A) · ε_machine
```

Rule of thumb: **you lose about log₁₀ κ(A) digits.** The 10×10 Hilbert matrix (hᵢⱼ = 1/(i+j+1)) has κ ≈ 1.6 × 10¹³, which leaves about 3 correct digits out of 16. That is a property of the *problem*, not a bug in your code.

**Stability** (a property of your algorithm) and **conditioning** (a property of the problem) are different things. LU with partial pivoting is stable in practice. It can't fix a badly conditioned A.

## 6. About that `1e-12` singularity threshold

The stub treats |pivot| < 10⁻¹² as singular. That is an *absolute* threshold, which means it depends on scale: multiply A by 10⁻¹⁵ and a perfectly good matrix looks singular. Production code compares against something relative, such as `ε · n · max|aᵢⱼ|`. Worth trying as a stretch goal.

## Exercises

1. Implement LU **without** pivoting, run it on the ε example with ε ∈ {10⁻⁴, 10⁻⁸, 10⁻¹², 10⁻¹⁶}, and plot the error in x₀. Where does it break?
2. Build Hilbert matrices for n = 2…12, solve Hx = H·1 (so the true x is all ones), and plot the error against n. Compare it with log₁₀ κ (use NumPy to get κ).
3. Your `mat_det` returns exactly 0.0 when a pivot falls below the threshold, but a nearly singular matrix that slips past it returns something like 10⁻¹⁷. Which answer is more honest, and what should an API report instead?
4. Count the FLOPs in your `mat_lu` and confirm ⅔n³ by timing n = 200, 400 and 800.
