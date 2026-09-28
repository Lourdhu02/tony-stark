/*
 * main.c: the read–eval loop (given code).
 */
#include "shell.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int main(void)
{
    int interactive = isatty(STDIN_FILENO);
    int status = 0, should_exit = 0;
    char *line = NULL;
    size_t cap = 0;

    shell_init();

    while (!should_exit) {
        reap_background();
        if (interactive) {
            fputs("\033[32mstark>\033[0m ", stdout);
            fflush(stdout);
        }

        ssize_t n = getline(&line, &cap, stdin);
        if (n < 0)
            break; /* EOF (Ctrl-D) */
        if (n > 0 && line[n - 1] == '\n')
            line[n - 1] = '\0';

        Pipeline p = {0};
        if (parse_line(line, &p) != 0) {
            fprintf(stderr, "stark: syntax error\n");
            status = 2;
            continue;
        }
        if (p.ncmds == 1 && !p.background && is_builtin(p.cmds[0].argv[0]))
            status = run_builtin(&p.cmds[0], status, &should_exit);
        else if (p.ncmds > 0)
            status = run_pipeline(&p);
        pipeline_free(&p);
    }

    if (interactive && !should_exit)
        putchar('\n');
    free(line);
    return status;
}
