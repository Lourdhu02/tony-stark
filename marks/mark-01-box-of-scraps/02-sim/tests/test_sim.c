#include "integrators.h"
#include "models.h"
#include "test.h"

static const Pendulum EARTH = {9.81, 1.0};

/* ── helpers ────────────────────────────────────────────────────────── */

static void decay_ode(double t, const double *y, double *dydt, size_t n, void *ctx)
{
    (void)t; (void)n; (void)ctx;
    dydt[0] = -y[0]; /* y' = -y  →  y(t) = e^-t */
}

static void unit_spring_accel(const double *q, double *acc, size_t d, void *ctx)
{
    (void)d; (void)ctx;
    acc[0] = -q[0]; /* x'' = -x  →  x(t) = cos t */
}

static double decay_error(void (*step)(ode_fn, double, double *, size_t, double, void *), double dt)
{
    double y = 1.0;
    int n = (int)lround(1.0 / dt);
    for (int i = 0; i < n; i++)
        step(decay_ode, i * dt, &y, 1, dt, NULL);
    return fabs(y - exp(-1.0));
}

static double verlet_error(double dt)
{
    double q = 1.0, v = 0.0;
    int n = (int)lround(2.0 / dt);
    for (int i = 0; i < n; i++)
        step_verlet(unit_spring_accel, &q, &v, 1, dt, NULL);
    return fabs(q - cos(2.0));
}

enum { EULER, RK4, VERLET };

/* Relative energy error of a pendulum released from θ0 at rest. */
static double energy_drift(int method, double theta0, double dt, double T, double *max_drift)
{
    Pendulum p = EARTH;
    double y[2] = {theta0, 0.0};
    double e0 = pendulum_energy(&p, theta0, 0.0);
    long n = lround(T / dt);
    int swung = 0;
    *max_drift = 0.0;
    REQUIRE(e0 > 0.0);

    for (long i = 0; i < n; i++) {
        if (method == EULER) step_euler(pendulum_ode, i * dt, y, 2, dt, &p);
        if (method == RK4) step_rk4(pendulum_ode, i * dt, y, 2, dt, &p);
        if (method == VERLET) step_verlet(pendulum_accel, &y[0], &y[1], 1, dt, &p);
        double d = fabs(pendulum_energy(&p, y[0], y[1]) - e0) / e0;
        if (d > *max_drift) *max_drift = d;
        if (y[0] < -0.5 * theta0) swung = 1;
    }
    REQUIRE(swung); /* a pendulum that never moves conserves energy perfectly */
    return (pendulum_energy(&p, y[0], y[1]) - e0) / e0;
}

/* Period from two consecutive upward zero crossings of θ (RK4, dt = 1e-3). */
static double measure_period(double theta0)
{
    Pendulum p = EARTH;
    double y[2] = {theta0, 0.0}, dt = 1e-3, t = 0.0;
    double crossings[2];
    int found = 0;
    while (found < 2 && t < 60.0) {
        double prev = y[0];
        step_rk4(pendulum_ode, t, y, 2, dt, &p);
        t += dt;
        if (prev < 0.0 && y[0] >= 0.0)
            crossings[found++] = t - dt * y[0] / (y[0] - prev); /* linear interpolation */
    }
    return found == 2 ? crossings[1] - crossings[0] : NAN;
}

/* ── models ─────────────────────────────────────────────────────────── */

TEST(pendulum_ode_values)
{
    Pendulum p = {9.81, 2.0};
    double y[2] = {M_PI / 2, 1.5}, dydt[2] = {0};
    pendulum_ode(0.0, y, dydt, 2, &p);
    CHECK_NEAR(dydt[0], 1.5, 1e-12);
    CHECK_NEAR(dydt[1], -9.81 / 2.0, 1e-12);

    double acc = 0.0;
    pendulum_accel(y, &acc, 1, &p);
    CHECK_NEAR(acc, -9.81 / 2.0, 1e-12);
}

TEST(pendulum_energy_values)
{
    Pendulum p = {9.81, 2.0};
    CHECK_NEAR(pendulum_energy(&p, 0.0, 0.0), 0.0, 1e-12);
    CHECK_NEAR(pendulum_energy(&p, M_PI / 2, 0.0), 9.81 * 2.0, 1e-12);
    CHECK_NEAR(pendulum_energy(&p, 0.0, 3.0), 0.5 * 4.0 * 9.0, 1e-12);
}

TEST(spring_ode_values)
{
    Spring s = {8.0, 2.0};
    double y[2] = {0.5, -1.0}, dydt[2] = {0};
    spring_ode(0.0, y, dydt, 2, &s);
    CHECK_NEAR(dydt[0], -1.0, 1e-12);
    CHECK_NEAR(dydt[1], -2.0, 1e-12);
}

/* ── convergence order ──────────────────────────────────────────────── */

TEST(euler_is_first_order)
{
    double ratio = decay_error(step_euler, 0.01) / decay_error(step_euler, 0.005);
    CHECK(ratio > 1.8 && ratio < 2.2);
}

TEST(rk4_is_fourth_order)
{
    double ratio = decay_error(step_rk4, 0.1) / decay_error(step_rk4, 0.05);
    CHECK(ratio > 14.0 && ratio < 18.0);
}

TEST(verlet_is_second_order)
{
    double ratio = verlet_error(0.02) / verlet_error(0.01);
    CHECK(ratio > 3.6 && ratio < 4.4);
}

TEST(rk4_matches_exact_spring)
{
    Spring s = {4.0, 1.0}; /* ω = 2 */
    double y[2] = {1.0, 0.0}, dt = 0.01;
    for (int i = 0; i < 1000; i++)
        step_rk4(spring_ode, i * dt, y, 2, dt, &s);
    CHECK_NEAR(y[0], cos(2.0 * 10.0), 1e-6);
    CHECK_NEAR(y[1], -2.0 * sin(2.0 * 10.0), 1e-6);
}

/* ── energy: the real lesson ────────────────────────────────────────── */

TEST(euler_pumps_energy_in)
{
    double max_drift;
    double drift = energy_drift(EULER, 1.0, 0.01, 10.0, &max_drift);
    CHECK(drift > 0.5); /* forward Euler spirals outward: +50% energy in 10 s */
}

TEST(rk4_energy_drift_under_0_1_percent)
{
    /* Mark I exit criterion: < 0.1% drift over 100 s. */
    double max_drift;
    energy_drift(RK4, 1.0, 0.05, 100.0, &max_drift);
    CHECK(max_drift < 1e-3);
}

TEST(verlet_energy_stays_bounded)
{
    /* 100,000 steps at a coarse dt: the energy error oscillates but never grows. */
    double max_drift;
    energy_drift(VERLET, 1.0, 0.1, 10000.0, &max_drift);
    CHECK(max_drift < 0.05);
}

TEST(symplectic_beats_rk4_long_run)
{
    double rk4_max, verlet_max;
    double rk4_final = energy_drift(RK4, 1.0, 0.1, 10000.0, &rk4_max);
    energy_drift(VERLET, 1.0, 0.1, 10000.0, &verlet_max);
    CHECK(rk4_final < -0.3);         /* RK4 slowly bleeds energy away... */
    CHECK(verlet_max < rk4_max / 5); /* ...a 2nd-order symplectic method doesn't */
}

/* ── physics sanity ─────────────────────────────────────────────────── */

TEST(small_angle_period)
{
    double t0 = 2.0 * M_PI * sqrt(EARTH.L / EARTH.g);
    CHECK_NEAR(measure_period(0.01), t0, t0 * 1e-3);
}

TEST(large_angle_period)
{
    /* At θ0 = 90° the small-angle formula is 18% off: T = 1.18034 · T0 */
    double t0 = 2.0 * M_PI * sqrt(EARTH.L / EARTH.g);
    CHECK_NEAR(measure_period(M_PI / 2), 1.18034 * t0, t0 * 1e-3);
}

int main(void)
{
    RUN(pendulum_ode_values);
    RUN(pendulum_energy_values);
    RUN(spring_ode_values);
    RUN(euler_is_first_order);
    RUN(rk4_is_fourth_order);
    RUN(verlet_is_second_order);
    RUN(rk4_matches_exact_spring);
    RUN(euler_pumps_energy_in);
    RUN(rk4_energy_drift_under_0_1_percent);
    RUN(verlet_energy_stays_bounded);
    RUN(symplectic_beats_rk4_long_run);
    RUN(small_angle_period);
    RUN(large_angle_period);
    return TEST_REPORT("sim");
}
