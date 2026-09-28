# Field manual 05 · Processes, pipes and signals

> Used by: `04-shell` · Later: Mark III (you'll implement `fork` in a kernel), Mark VI/VII (ROS 2 is many processes talking), Mark IX (supervising tool processes)

**The one idea:** a process is a running program plus a **table of open files**. A shell is a program that clones itself, rewires that table, and becomes something else.

## 1. The three system calls

```text
fork()        duplicate me. Returns 0 in the child and the child's pid in the parent.
              The child gets a copy of memory (copy-on-write) and of the fd table.

execvp(p, v)  replace my program image with p. The pid and the fd table survive.
              It never returns on success.

waitpid(p)    block until child p exits, then collect its status.
              Until you do, the dead child is a ZOMBIE.
```

Running any command is `fork`, then `exec` in the child, then `waitpid` in the parent. The split is the genius of Unix: **between fork and exec, the child can rearrange its own file descriptors**, and that's how redirection and pipes work without the program knowing.

## 2. File descriptors

Every process has a table. The numbers index into kernel file objects:

```text
fd table (per process)          kernel
┌────┬────────────┐
│ 0  │ ───────────┼──▶  terminal (stdin)
│ 1  │ ───────────┼──▶  terminal (stdout)
│ 2  │ ───────────┼──▶  terminal (stderr)
│ 3  │ ───────────┼──▶  out.txt
└────┴────────────┘
```

`dup2(3, 1)` makes fd 1 point wherever fd 3 points. After that, `printf` writes to `out.txt`. Then `close(3)` to tidy up. That's `> out.txt`.

| Redirect | `open()` flags |
|---|---|
| `< f` | `O_RDONLY` → `dup2(fd, 0)` |
| `> f` | `O_WRONLY \| O_CREAT \| O_TRUNC`, mode `0644` → `dup2(fd, 1)` |
| `>> f` | `O_WRONLY \| O_CREAT \| O_APPEND`, mode `0644` → `dup2(fd, 1)` |

## 3. Pipes, step by step: `ls | wc -l`

```text
1. shell: pipe(p)         p[0] = read end, p[1] = write end

2. fork → child A (ls):   dup2(p[1], 1)   stdout → pipe
                          close(p[0]); close(p[1])
                          execvp("ls")

3. fork → child B (wc):   dup2(p[0], 0)   stdin ← pipe
                          close(p[0]); close(p[1])
                          execvp("wc")

4. shell:                 close(p[0]); close(p[1])     ◀── critical
                          waitpid(A); waitpid(B)
```

```text
   ┌──────┐  fd1     ┌────────────┐     fd0  ┌──────┐
   │  ls  │ ───────▶ │ pipe (64K) │ ───────▶ │  wc  │
   └──────┘          └────────────┘          └──────┘
```

### The EOF rule

A reader sees EOF **only when every write end of the pipe is closed, in every process.** If the shell (or `wc` itself) keeps `p[1]` open, `wc` waits forever for input that never comes. This is the **#1 shell bug**. Leak one write end in the parent and five of the shell tests hang until they time out.

### The deadlock rule

A Linux pipe buffers 64 KiB by default. If the shell starts `ls`, **waits for it**, and only *then* starts `wc`, then any output over 64 KiB fills the pipe: `ls` blocks on write, and the shell blocks on `waitpid(ls)`. **Start every stage, then wait for all of them.** The test `pipe with a big payload` pushes about 1.3 MB through the pipe to catch this.

## 4. Zombies and orphans

- **Zombie:** a child that exited but hasn't been `wait`ed on. It holds a pid and its exit status. `ps` shows it as `Z`.
- **Orphan:** a child whose parent died. It gets re-parented to init (or a subreaper), which reaps it.
- **Background jobs** (`sleep 5 &`): don't wait for them. Call `waitpid(-1, &st, WNOHANG)` in a loop before each prompt to reap whatever has finished. The `no zombies left behind` test checks `ps` for `Z` states.

## 5. Signals

A signal is an asynchronous notification from the kernel to a process. Each signal has a **disposition**: default, ignore, or a handler.

| Signal | Sent when | Default |
|---|---|---|
| `SIGINT` | Ctrl-C | terminate |
| `SIGQUIT` | Ctrl-\ | terminate + core dump |
| `SIGTSTP` | Ctrl-Z | stop |
| `SIGCHLD` | a child changed state | ignore |
| `SIGPIPE` | writing to a pipe with no readers | terminate |

**Ctrl-C goes to the whole foreground process group**, which means the shell *and* its child. You want the child to die and the shell to live. So:

1. The shell ignores `SIGINT` (`sigaction` with `SIG_IGN`, in `shell_init`).
2. The child restores `SIG_DFL` **after fork, before exec.**

Step 2 is necessary because of a subtle POSIX rule: **across `exec`, handled signals reset to default, but *ignored* signals stay ignored.** Forget step 2 and your child can't be interrupted. The `Ctrl-C kills the child` test times out in exactly that case.

## 6. Why `cd` must be a builtin

The working directory is **per-process state**. If `cd` ran in a forked child, the child would change *its* directory and exit, and the shell wouldn't have moved. Anything that changes the shell's own state (`cd`, `exit`, `export`) must run inside the shell process.

## 7. Watch it happen

```sh
strace -f -e trace=fork,clone,execve,pipe2,dup2,close,wait4 ./build/stark-shell
```

`-f` follows children. Type `ls | wc -l` and read the exact sequence of syscalls. It should match section 3.

## Exercises

1. Draw the fd tables of the shell, `ls` and `wc` at every step of section 3. Mark the moment each pipe end's reference count drops to zero.
2. Remove the parent's `close(p[1])` and observe the hang with `strace`. Which syscall is `wc` blocked in?
3. Make `sleep 100 | cat` interruptible with Ctrl-C in an *interactive* terminal. You'll need process groups (`setpgid`) and `tcsetpgrp`, which is the start of job control.
4. What happens to `yes | head -1`? Which signal ends `yes`, and why is that the correct behaviour?
