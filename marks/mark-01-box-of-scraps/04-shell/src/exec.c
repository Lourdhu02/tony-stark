/*
 * exec.c: processes, pipes and signals.
 *
 * Man pages you'll live in: fork(2), execvp(3), pipe(2), dup2(2),
 * waitpid(2), open(2), sigaction(2).
 *
 * The classic bugs, so you recognise them when they bite:
 *   - forgetting to close unused pipe ends → the reader never sees EOF, hangs
 *   - waiting for stage 1 before starting stage 2 → deadlock on a full pipe
 *   - children inheriting the shell's "ignore SIGINT" → Ctrl-C can't stop them
 */
#include "shell.h"

#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

void shell_init(void)
{
    /* TODO: ignore SIGINT in the shell (children must restore SIG_DFL). */
}

int run_pipeline(const Pipeline *p)
{
    /* TODO */
    (void)p;
    return 1;
}

void reap_background(void)
{
    /* TODO: waitpid(-1, ..., WNOHANG) in a loop. */
}
