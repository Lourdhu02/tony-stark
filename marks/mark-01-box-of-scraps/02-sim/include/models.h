/*
 * models.h: physical systems to simulate.
 */
#ifndef MODELS_H
#define MODELS_H

#include <stddef.h>

/*
 * Simple pendulum: a point mass on a massless rod of length L.
 *   θ'' = -(g / L) · sin θ
 * First-order state: y = [θ, ω], where ω = θ'.
 */
typedef struct {
    double g; /* m/s² */
    double L; /* m */
} Pendulum;

void pendulum_ode(double t, const double *y, double *dydt, size_t n, void *ctx);
void pendulum_accel(const double *q, double *acc, size_t d, void *ctx);

/* Total mechanical energy per unit mass, with θ = 0 as the zero of potential energy. */
double pendulum_energy(const Pendulum *p, double theta, double omega);

/*
 * Mass–spring oscillator: m·x'' = -k·x.
 * First-order state: y = [x, v]. Exact solution for x(0)=1, v(0)=0:
 *   x(t) = cos(√(k/m) · t)
 */
typedef struct {
    double k; /* N/m */
    double m; /* kg */
} Spring;

void spring_ode(double t, const double *y, double *dydt, size_t n, void *ctx);

#endif /* MODELS_H */
