# minishell

A POSIX-style command interpreter written from scratch in C — tokenizer, recursive-descent
parser, AST, and executor — reproducing the behaviour of `bash` for a substantial subset of
the shell language.

---

## About

`minishell` is the largest project of the [42 School](https://42.fr) core curriculum before
specialisation, and a team project. The brief is simply *"write a shell"*, and the
reference implementation you are measured against is `bash` itself: for every construct in
the subject, your shell must produce the same output, the same exit status, and the same
signal behaviour.

That turns out to involve four distinct pieces of machinery:

1. A **tokenizer** that respects quoting, because quoting is what decides whether a
   character is syntax or data.
2. A **recursive-descent parser** that builds an abstract syntax tree, because pipes,
   logical operators, and subshells have precedence and nesting.
3. An **expansion pass** for variables and wildcards, run at the right moment and on the
   right nodes — after quote analysis, before word splitting.
4. An **executor** that walks the tree, forking, plumbing file descriptors between
   processes, and reporting exit statuses back up.

Plus signal handling that behaves differently in the interactive prompt, inside a running
child, and inside a heredoc.

Written in C99 under `-Wall -Wextra -Werror`, conforming to the **Norm**, 42's coding
standard.

---

## Build

```bash
make          # builds ./minishell
make clean    # removes object files
make fclean   # removes object files and the binary
make re       # fclean + make
```

### Dependencies

`minishell` links against **GNU Readline** for line editing and history.

```bash
# Debian / Ubuntu
sudo apt install libreadline-dev

# macOS
brew install readline
```

The Makefile detects macOS via `uname -s` and picks up Homebrew's `readline` prefix
automatically. The bundled `libft` under `libft/` is built as part of the same run.

---

## Usage

```bash
./minishell
```

You get an interactive `miniHell$ ` prompt with history and line editing. `exit`, or
`Ctrl-D` on an empty line, leaves the shell.

```
miniHell$ echo "quoted $USER" 'literal $USER'
quoted eahmeti literal $USER
miniHell$ cd /tmp && pwd
/tmp
miniHell$ (cd / && pwd) ; pwd
/
/tmp
miniHell$ ls /nonexistent
ls: cannot access '/nonexistent': No such file or directory
miniHell$ echo $?
2
```

---

## What it supports

### Commands and operators

| Construct | Example |
|---|---|
| Simple commands with arguments | `ls -la /tmp` |
| Absolute and relative paths | `/bin/echo hi` · `./a.out` |
| `PATH` resolution | `grep foo file` |
| Pipes | `cat file \| grep foo \| wc -l` |
| Logical AND | `make && ./program` |
| Logical OR | `make \|\| echo "build failed"` |
| Subshells | `(cd /tmp && pwd) ; pwd` |

### Redirections

| Operator | Effect |
|---|---|
| `<` | Redirect standard input from a file |
| `>` | Redirect standard output to a file, truncating it |
| `>>` | Redirect standard output to a file, appending |
| `<<` | Heredoc — read until a delimiter line |

Heredocs expand `$VARIABLES` in their body unless the delimiter is quoted, and can be
interrupted with `Ctrl-C` without taking the shell down with them.

### Quoting

| Form | Behaviour |
|---|---|
| `'single'` | Everything is literal — no expansion of any kind |
| `"double"` | `$` expansion happens; everything else is literal |
| unquoted | Full expansion, plus wildcard matching |

### Expansions

| Form | Meaning |
|---|---|
| `$VAR` | Environment variable |
| `$?` | Exit status of the last command |
| `*` | Wildcard, expanded against the current directory |

### Builtins

All seven required builtins, implemented in-process:

| Builtin | Notes |
|---|---|
| `echo` | With the `-n` flag |
| `cd` | Relative and absolute paths; updates `PWD` and `OLDPWD` |
| `pwd` | No options |
| `export` | With no arguments, prints the environment in `declare -x` form |
| `unset` | Removes variables from the environment |
| `env` | No options, no arguments |
| `exit` | With an optional numeric status |

Builtins run in the parent process when they are alone on the command line — so `cd` and
`export` actually affect the shell — and in the child when they appear in a pipeline, which
is what `bash` does.

### Signals

| Input | Interactive prompt | Running a command |
|---|---|---|
| `Ctrl-C` | New prompt on a fresh line | Interrupts the child |
| `Ctrl-D` | Exits the shell | Sends EOF to the child |
| `Ctrl-\` | Ignored | Quits the child with a core dump message |

---

## Architecture

```
input string
     │
     ▼
┌──────────────┐   quote-aware scanning: words, operators, parentheses
│  tokenizer   │   each word tagged T_NO_QUOTE / T_S_QUOTE / T_D_QUOTE
└──────┬───────┘
       ▼
┌──────────────┐   recursive descent, lowest precedence first:
│    parser    │   && || → | → command → redirections → subshell
└──────┬───────┘   produces an AST of AST_AND / AST_OR / AST_PIPE /
       │           AST_CMD / AST_REDIR / AST_SUB_SHELL nodes
       ▼
┌──────────────┐   $VAR and $? substituted per word, according to the
│  expansion   │   quote tag recorded by the tokenizer; then wildcards
└──────┬───────┘   matched against the current directory
       ▼
┌──────────────┐   walk the tree: fork, dup2, pipe, execve,
│   executor   │   waitpid, propagate exit status
└──────────────┘
```

Keeping the quote tag on each token is what makes the expansion pass correct: by the time
expansion runs, the quotes themselves are gone, but the tokenizer has already recorded
whether each word was single-quoted, double-quoted, or bare — and that is exactly the
information needed to decide what may be expanded.

---

## Project structure

```
minishell/
├── src/
│   ├── main/           # entry point, shell init, signal handlers
│   ├── tokenizer/      # word/quote scanning, operator recognition
│   ├── parsing/        # recursive-descent parser, AST, parentheses, redirections
│   ├── expand_env/     # $VAR, $?, expansion inside heredocs and redirection targets
│   ├── wildcards/      # * matching and argument-vector rebuilding
│   ├── builtins/       # echo cd pwd export unset env exit
│   └── exec/           # executor, pipes, redirections, heredocs, process control
├── libft/              # bundled
├── minishell.h
└── Makefile
```

---

## Concepts covered

- Lexical analysis with context-sensitive quoting rules
- Recursive-descent parsing and operator precedence
- Abstract syntax trees, and recursive evaluation over them
- Process creation and control: `fork`, `execve`, `waitpid`, exit-status propagation
- File-descriptor plumbing: `pipe`, `dup2`, and closing every descriptor you did not mean
  to inherit
- Signal handling that changes meaning depending on shell state
- Environment management as a mutable array of strings
- Readline integration, and freeing what Readline allocates

---

## A note on the commit history

This project was originally developed and submitted on **Vogsphere**, the self-hosted
Git server 42 uses internally for project submission and peer evaluation. Vogsphere
repositories are provisioned per student, per project, and live on the school's
infrastructure rather than on a public forge — so what you see here is the final graded
snapshot imported into GitHub, not the day-to-day commit history. The code is exactly
what was submitted and defended during evaluation.

---

## License

Educational project, released as-is for reference.
