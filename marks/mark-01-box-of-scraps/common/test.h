/*
 * test.h: a tiny, fork-isolated test harness.
 *
 * Every RUN(test) executes in its own child process with a timeout, so a
 * segfault or an infinite loop in one test cannot take down the suite.
 * This matters here: half-finished allocators and shells crash a lot.
 *
 *   TEST(adds_numbers) { CHECK(1 + 1 == 2); }
 *   int main(void) { RUN(adds_numbers); return TEST_REPORT("demo"); }
 *
 * CHECK(cond)             record a failure and keep going
 * REQUIRE(cond)           record a failure and stop this test (use before dereferencing)
 * CHECK_NEAR(a, b, tol)   |a - b| <= tol
 */
#ifndef STARK_TEST_H
#define STARK_TEST_H

#include <math.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#ifndef TEST_TIMEOUT_SEC
#define TEST_TIMEOUT_SEC 10
#endif

static int t_failed_;
static int t_pass_, t_total_;

#define T_GREEN_ (isatty(STDOUT_FILENO) ? "\033[32m" : "")
#define T_RED_   (isatty(STDOUT_FILENO) ? "\033[31m" : "")
#define T_DIM_   (isatty(STDOUT_FILENO) ? "\033[2m" : "")
#define T_RESET_ (isatty(STDOUT_FILENO) ? "\033[0m" : "")

#define CHECK(cond)                                                          \
    do {                                                                     \
        if (!(cond)) {                                                       \
            fprintf(stderr, "         %s:%d: CHECK(%s)\n", __FILE__,         \
                    __LINE__, #cond);                                        \
            t_failed_ = 1;                                                   \
        }                                                                    \
    } while (0)

#define REQUIRE(cond)                                                        \
    do {                                                                     \
        if (!(cond)) {                                                       \
            fprintf(stderr, "         %s:%d: REQUIRE(%s)\n", __FILE__,       \
                    __LINE__, #cond);                                        \
            fflush(NULL);                                                    \
            _exit(1);                                                        \
        }                                                                    \
    } while (0)

#define CHECK_NEAR(a, b, tol)                                                \
    do {                                                                     \
        double a_ = (a), b_ = (b), tol_ = (tol);                             \
        if (!(fabs(a_ - b_) <= tol_)) {                                      \
            fprintf(stderr, "         %s:%d: |%s - %s| = |%.9g - %.9g| > %g\n", \
                    __FILE__, __LINE__, #a, #b, a_, b_, tol_);               \
            t_failed_ = 1;                                                   \
        }                                                                    \
    } while (0)

#define TEST(name) static void name(void)
#define RUN(name) t_run_(#name, name)
#define TEST_REPORT(suite) t_report_(suite)

static void t_run_(const char *name, void (*fn)(void))
{
    fflush(NULL);
    pid_t pid = fork();
    if (pid < 0) {
        perror("fork");
        exit(2);
    }
    if (pid == 0) {
        alarm(TEST_TIMEOUT_SEC);
        fn();
        fflush(NULL);
        _exit(t_failed_ ? 1 : 0);
    }

    int status = 0;
    waitpid(pid, &status, 0);
    t_total_++;

    const char *why = NULL;
    if (WIFSIGNALED(status))
        why = WTERMSIG(status) == SIGALRM ? "timeout" : strsignal(WTERMSIG(status));
    else if (WEXITSTATUS(status) != 0)
        why = "failed";

    if (!why) {
        t_pass_++;
        printf("  %s[ OK ]%s %s\n", T_GREEN_, T_RESET_, name);
    } else {
        printf("  %s[FAIL]%s %s %s(%s)%s\n", T_RED_, T_RESET_, name, T_DIM_, why, T_RESET_);
    }
    fflush(stdout);
}

static int t_report_(const char *suite)
{
    enum { WIDTH = 24 };
    int filled = t_total_ ? t_pass_ * WIDTH / t_total_ : 0;

    printf("\n  %s%-8s%s ", T_GREEN_, suite, T_RESET_);
    for (int i = 0; i < WIDTH; i++)
        fputs(i < filled ? "█" : "░", stdout);
    printf(" %d/%d\n", t_pass_, t_total_);

    /* Machine-readable line consumed by tools/armory.sh */
    printf("RESULT %d %d\n", t_pass_, t_total_);
    return t_pass_ == t_total_ ? 0 : 1;
}

#endif /* STARK_TEST_H */
