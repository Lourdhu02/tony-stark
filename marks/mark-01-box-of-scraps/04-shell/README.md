# 04 · shell

> Weeks 5–6 · 33 tests (10 parser + 23 end-to-end) · Field manual: [05 processes, pipes and signals](../notes/05-processes-pipes-signals.md)

`stark-shell`: a real Unix shell. When `ls | grep '\.c$' | wc -l > out.txt` works, you understand how every program on your machine gets started.

### `> cat grammar`

```text
line      := pipeline [ '&' ]
pipeline  := command { '|' command }
command   := word { word | '<' word | '>' word | '>>' word }
word      := unquoted chars | '...' | "..."        (glued: a"b c"d → "ab cd")
special   := whitespace | < > &                    (outside quotes only)
```

No variables, globbing or escapes in the core spec. Those are stretch goals.

### `> cat spec`

| Piece | Contract | Tests |
|---|---|---|
| `parse_line` | the grammar above → `Pipeline`; −1 on syntax error | 10 unit tests in `test_parse.c` |
| `run_pipeline` | fork/exec every stage, wire pipes and redirects, wait for all (foreground) or print `[pid]` to stderr (background); unknown command → `stark: command not found: X` and status 127 | pipes, redirection, errors |
| `cd`, `pwd`, `exit [n]` | run in the shell process; `cd` with no argument goes to `$HOME` | builtins |
| `shell_init`, `reap_background` | the shell survives Ctrl-C and the child doesn't; no zombies | signals, processes |

### `> cat order_of_attack`

```text
week 5: tokenizer (words, quotes, operators) → parser → all 10 parse tests
        → single command via fork/execvp/waitpid → builtins
week 6: redirection → 2-stage pipe → N-stage pipe → background + reaping → signals
```

### `> ./hints`

<details><summary><b>parser</b> · hint 1</summary>

Split the work into two passes. First, a **tokenizer** that emits `WORD(text)`, `PIPE`, `LT`, `GT`, `GTGT` and `AMP`, with quote handling finished *inside* the tokenizer, so the parser never sees a quote character. Second, a **parser** that walks the tokens.
</details>

<details><summary><b>parser</b> · hint 2</summary>

In the tokenizer, a word continues until unquoted whitespace or an unquoted special character. When you meet `'` or `"`, copy characters up to the matching quote into the *same* word buffer. That's all it takes to make `a"b c"d` → `ab cd` work.
</details>

<details><summary><b>parser</b> · hint 3 (syntax errors)</summary>

Only one `&` is legal, and only as the final token. Strip it first and set `background`. Then every stage must have at least one word, and every redirect operator must be followed by a WORD. Check all of that with the token list in hand.
</details>

<details><summary><b>pipes</b> · hint</summary>

Keep one variable, `prev_read`, which is the read end from the previous stage, or −1. For stage i: if it's not the last stage, create `pipe(fd)`. In the child, `dup2(prev_read, 0)` and `dup2(fd[1], 1)` as applicable, then close *everything* pipe-related. In the parent, close `prev_read` and `fd[1]`, then set `prev_read = fd[0]`. Apply file redirects **after** the pipe dup2s, so `cmd > f | x` sends output to f.
</details>

<details><summary><b>signals</b> · hint</summary>

In `shell_init`, ignore SIGINT and SIGQUIT. In the child, between fork and exec, restore both to `SIG_DFL`. Re-read manual 05 §5 for *why* the child step is mandatory.
</details>

### `> cat debugging.txt`

- **A test hangs?** A pipe write end is open somewhere. Run `strace -f` on a 2-stage pipe and check every `close`.
- **Output appears twice?** You didn't `_exit` after a failed `execvp`, so the child fell back into the shell's loop.
- **Tests pass alone but not in the suite?** Look for zombies or leaked fds. Check `ls /proc/$(pgrep stark-shell)/fd`.

### `> cat stretch.txt`

- `$VAR` and `$?` expansion; `&&` / `||`; `;`.
- Job control: `jobs`, `fg`, `bg`, Ctrl-Z, process groups and `tcsetpgrp`.
- Line editing and history using raw terminal mode (`termios`), with no readline.
- Run your shell as your login shell for a day. What breaks first?
