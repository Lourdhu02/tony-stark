/*
 * shell.h: stark-shell, a small Unix shell.
 *
 *   line      := pipeline [ '&' ]
 *   pipeline  := command { '|' command }
 *   command   := word { word | '<' word | '>' word | '>>' word }
 *   word      := run of non-special characters, and '...' or "..." quoted
 *                text (quotes can be glued: a"b c"d is one word: ab cd)
 *
 * Special characters outside quotes: whitespace | < > &
 */
#ifndef SHELL_H
#define SHELL_H

typedef struct {
    char **argv;    /* NULL-terminated; argv[0] is the program */
    int argc;
    char *in_file;  /* `< file`, or NULL */
    char *out_file; /* `> file` or `>> file`, or NULL */
    int append;     /* 1 for >> */
} Command;

typedef struct {
    Command *cmds;  /* cmds[0] | cmds[1] | ... */
    int ncmds;      /* 0 for a blank line */
    int background; /* line ended with & */
} Pipeline;

/* ── parse.c (yours) ───────────────────────────────────────────────── */

/*
 * Parse one line. Returns 0 on success (a blank line gives ncmds == 0),
 * or -1 on a syntax error: empty pipeline stage, redirection without a
 * file, '&' anywhere but the end, unterminated quote. On error, *out
 * holds nothing that needs freeing.
 */
int parse_line(const char *line, Pipeline *out);
void pipeline_free(Pipeline *p);

/* ── exec.c (yours) ────────────────────────────────────────────────── */

/* Called once at startup. The shell itself must survive Ctrl-C (SIGINT). */
void shell_init(void);

/*
 * Fork/exec every stage, wire pipes and redirections, then either wait
 * for all stages (foreground; return the last stage's exit status) or
 * print "[pid]" to stderr and return 0 immediately (background).
 * A command that can't be executed prints
 *     stark: command not found: NAME
 * to stderr and exits with status 127.
 */
int run_pipeline(const Pipeline *p);

/* Reap finished background jobs (no zombies!). Called before every prompt. */
void reap_background(void);

/* ── builtins.c (yours) ────────────────────────────────────────────── */

int is_builtin(const char *name); /* cd, pwd, exit */

/*
 * Run a builtin in the shell process itself (why must `cd` be a builtin?).
 * Returns its exit status. `exit [n]` sets *should_exit = 1, and the shell
 * exits with n (default: last_status).
 */
int run_builtin(const Command *cmd, int last_status, int *should_exit);

#endif /* SHELL_H */
