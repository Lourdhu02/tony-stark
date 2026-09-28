/*
 * models.c: your implementation.
 */
#include "models.h"

#include <math.h>

void pendulum_ode(double t, const double *y, double *dydt, size_t n, void *ctx)
{
    /* TODO: dθ/dt = ω, dω/dt = -(g/L)·sin θ. ctx is a Pendulum *. */
    (void)t;
    (void)y;
    (void)n;
    (void)ctx;
    dydt[0] = 0.0;
    dydt[1] = 0.0;
}

void pendulum_accel(const double *q, double *acc, size_t d, void *ctx)
{
    /* TODO: acc[0] = θ'' as a function of q[0] = θ. */
    (void)q;
    (void)d;
    (void)ctx;
    acc[0] = 0.0;
}

double pendulum_energy(const Pendulum *p, double theta, double omega)
{
    /* TODO: kinetic (½·L²·ω²) + potential (g·L·(1 - cos θ)). */
    (void)p;
    (void)theta;
    (void)omega;
    return 0.0;
}

void spring_ode(double t, const double *y, double *dydt, size_t n, void *ctx)
{
    /* TODO: dx/dt = v, dv/dt = -(k/m)·x. ctx is a Spring *. */
    (void)t;
    (void)y;
    (void)n;
    (void)ctx;
    dydt[0] = 0.0;
    dydt[1] = 0.0;
}
