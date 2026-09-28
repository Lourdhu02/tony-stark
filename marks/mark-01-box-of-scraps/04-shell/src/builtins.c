/*
 * builtins.c: commands that must run inside the shell process.
 */
#include "shell.h"

#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int is_builtin(const char *name)
{
    /* TODO: cd, pwd, exit */
    (void)name;
    return 0;
}

int run_builtin(const Command *cmd, int last_status, int *should_exit)
{
    /*
     * TODO:
     *   cd [dir]   chdir(dir or $HOME); on failure: "stark: cd: DIR: <strerror>" → 1
     *   pwd        print getcwd()
     *   exit [n]   *should_exit = 1; return n (or last_status)
     */
    (void)cmd;
    (void)last_status;
    (void)should_exit;
    return 1;
}
