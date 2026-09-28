/*
 * integrators.h: numerical ODE solvers.
 *
 * A first-order system y' = f(t, y) is described by an ode_fn that writes
 * dy/dt into `dydt`. Second-order systems q'' = a(q) (most of mechanics)
 * can also be described by an accel_fn, which symplectic integrators need.
 *
 * All steppers advance the state in place by one step of size dt.
 * State dimension is at most SIM_MAX_DIM, so scratch buffers can live on
 * the stack: no malloc in the hot loop.
 */
#ifndef INTEGRATORS_H
#define INTEGRATORS_H

#include <stddef.h>

#define SIM_MAX_DIM 64

typedef void (*ode_fn)(double t, const double *y, double *dydt, size_t n, void *ctx);
typedef void (*accel_fn)(const double *q, double *acc, size_t d, void *ctx);

/* Explicit (forward) Euler: 1st order. */
void step_euler(ode_fn f, double t, double *y, size_t n, double dt, void *ctx);

/* Classic 4th-order Runge–Kutta. */
void step_rk4(ode_fn f, double t, double *y, size_t n, double dt, void *ctx);

/*
 * Velocity Verlet: 2nd order and symplectic. q and v each have d entries.
 *   v_half = v + a(q)·dt/2
 *   q      = q + v_half·dt
 *   v      = v_half + a(q)·dt/2
 */
void step_verlet(accel_fn a, double *q, double *v, size_t d, double dt, void *ctx);

#endif /* INTEGRATORS_H */
