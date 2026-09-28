/*
 * pendulum: simulate a pendulum and stream CSV to stdout (given code).
 *
 *   ./build/pendulum --method rk4 --dt 0.01 --T 20 --theta0 1.0 > rk4.csv
 *
 * Columns: t, theta, omega, energy, drift (relative energy error).
 * A one-line summary goes to stderr.
 */
#include "integrators.h"
#include "models.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void usage(const char *argv0)
{
    fprintf(stderr,
            "usage: %s [--method euler|rk4|verlet] [--dt s] [--T s] [--theta0 rad]\n"
            "          [--g m/s^2] [--L m] [--every n]\n",
            argv0);
    exit(2);
}

int main(int argc, char **argv)
{
    const char *method = "rk4";
    double dt = 0.01, T = 20.0, theta0 = 1.0;
    long every = 1;
    Pendulum p = {9.81, 1.0};

    for (int i = 1; i < argc; i++) {
        if (i + 1 >= argc) usage(argv[0]);
        if (!strcmp(argv[i], "--method")) method = argv[++i];
        else if (!strcmp(argv[i], "--dt")) dt = atof(argv[++i]);
        else if (!strcmp(argv[i], "--T")) T = atof(argv[++i]);
        else if (!strcmp(argv[i], "--theta0")) theta0 = atof(argv[++i]);
        else if (!strcmp(argv[i], "--g")) p.g = atof(argv[++i]);
        else if (!strcmp(argv[i], "--L")) p.L = atof(argv[++i]);
        else if (!strcmp(argv[i], "--every")) every = atol(argv[++i]);
        else usage(argv[0]);
    }
    if (dt <= 0 || T <= 0 || every < 1) usage(argv[0]);

    double y[2] = {theta0, 0.0};
    double e0 = pendulum_energy(&p, theta0, 0.0);
    double max_drift = 0.0;
    long n = lround(T / dt);

    printf("t,theta,omega,energy,drift\n");
    for (long i = 0; i <= n; i++) {
        double t = i * dt;
        double e = pendulum_energy(&p, y[0], y[1]);
        double drift = e0 != 0.0 ? (e - e0) / e0 : 0.0;
        if (fabs(drift) > max_drift) max_drift = fabs(drift);
        if (i % every == 0)
            printf("%.6f,%.9f,%.9f,%.9f,%.3e\n", t, y[0], y[1], e, drift);
        if (i == n) break;

        if (!strcmp(method, "euler")) step_euler(pendulum_ode, t, y, 2, dt, &p);
        else if (!strcmp(method, "rk4")) step_rk4(pendulum_ode, t, y, 2, dt, &p);
        else if (!strcmp(method, "verlet")) step_verlet(pendulum_accel, &y[0], &y[1], 1, dt, &p);
        else usage(argv[0]);
    }

    fprintf(stderr, "%-6s dt=%g T=%g  max |ΔE/E0| = %.3e\n", method, dt, T, max_drift);
    return 0;
}
