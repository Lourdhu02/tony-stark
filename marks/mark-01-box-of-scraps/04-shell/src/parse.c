/*
 * parse.c: your tokenizer and parser.
 *
 * Two passes keep this sane:
 *   1. tokenize: turn the line into WORD / PIPE / LT / GT / GTGT / AMP tokens,
 *      resolving quotes into plain word text
 *   2. parse: walk the tokens, filling Commands and splitting on PIPE
 *
 * Everything you store in the Pipeline must be heap-allocated (strdup),
 * because the caller reuses the line buffer.
 */
#include "shell.h"

#include <ctype.h>
#include <stdlib.h>
#include <string.h>

int parse_line(const char *line, Pipeline *out)
{
    /* TODO */
    (void)line;
    (void)out;
    return -1;
}

void pipeline_free(Pipeline *p)
{
    /* TODO: free every argv string, argv arrays, redirection names, cmds. */
    (void)p;
}
