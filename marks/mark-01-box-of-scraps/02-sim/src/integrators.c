/*
 * integrators.c: your implementation.
 *
 * Derive each method on paper first. The tests check the *order* of each
 * method: halving dt should cut the error by 2x (Euler), 4x (Verlet) or 16x (RK4).
 */
#include "integrators.h"

void step_euler(ode_fn f, double t, double *y, size_t n, double dt, void *ctx)
{
    /* TODO: y ← y + dt · f(t, y) */
    (void)f;
    (void)t;
    (void)y;
    (void)n;
    (void)dt;
    (void)ctx;
}

void step_rk4(ode_fn f, double t, double *y, size_t n, double dt, void *ctx)
{
    /*
     * TODO:
     *   k1 = f(t,        y)
     *   k2 = f(t + dt/2, y + dt/2 · k1)
     *   k3 = f(t + dt/2, y + dt/2 · k2)
     *   k4 = f(t + dt,   y + dt   · k3)
     *   y ← y + dt/6 · (k1 + 2·k2 + 2·k3 + k4)
     * Use stack arrays of size SIM_MAX_DIM for k1..k4 and the temporary state.
     */
    (void)f;
    (void)t;
    (void)y;
    (void)n;
    (void)dt;
    (void)ctx;
}

void step_verlet(accel_fn a, double *q, double *v, size_t d, double dt, void *ctx)
{
    /* TODO: see the header. Note: a textbook version calls a() twice per step. */
    (void)a;
    (void)q;
    (void)v;
    (void)d;
    (void)dt;
    (void)ctx;
}
