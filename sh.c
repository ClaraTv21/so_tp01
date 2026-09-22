#include <assert.h>
#include <ctype.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

/*
Answers can be either in Portuguese or in English.
Respostas podem sem tanto em português como em inglês.
*/

/*
1. Term of commitment

The group members declare that all code developed for this project is their own.
The group members declare that they have not copied material from the Internet
nor obtained code from third parties.

2. Group members and allocation of effort

Fill in the lines below with the name and email of the group members.
Replace XX with the contribution of each group member in the development of the work.

Name <clara21ufmg@gmail.com> 50%
Name <email@ufmg.br> 50%

3. Solutions

Briefly describe the solutions implemented for this project and justify their choices.

a) Motivação e decisões de projeto



b) Resumo da implementação



c) Casos ambíguos



d) Testes realizados



4. Bibliographic references

Add the bibliographic references here.

*/

/****************************************************************
 * Simplified xv6 Shell
 *
 * This code was adapted from the UNIX xv6 code and material from
 * the MIT Operating Systems course (6.828).
 *
 * Opcao 2: extended with built-ins (cd, exit, export) and
 * expansion of environment variables ($VAR).
 ***************************************************************/

#define MAXARGS 10

/* Every command has a type. After identifying the command's type,
    the code converts a *cmd into the specific command type. */
struct cmd {
    int type; /* ' ' (exec)
                 '|' (pipe)
                 '<' or '>' (redirection) */
};

struct execcmd {
    int type;             // ' ' (exec)
    char *argv[MAXARGS];  // Arguments for the command to be executed
};

struct redircmd {
    int type;         // < or > (redirection)
    struct cmd *cmd;  // The command to execute (e.g., an execcmd)
    char *file;       // The input or output file
    int mode;         // The mode in which the file should be opened
    int fd;           // The file descriptor number to be used
};

struct pipecmd {
    int type;           // | (pipe)
    struct cmd *left;   // Left side of the pipe
    struct cmd *right;  // Right side of the pipe
};

int fork1(void);                                        // Fork but exit if an error occurs
struct cmd *parsecmd(char *);                           // Process the command line
void handle_simple_cmd(struct execcmd *ecmd);           // Handle simple commands
void handle_redirection(struct redircmd *rcmd);         // Handle redirection
void handle_pipe(struct pipecmd *pcmd, int *p, int r);  // Handle pipes
int handle_builtin(struct execcmd *ecmd);               // Handle cd, exit, export
char *expand_vars(char *token);                          // Expand $VAR inside a token

/* Execute the command cmd. It never returns. */
void runcmd(struct cmd *cmd) {
    int p[2], r;
    struct execcmd *ecmd;
    struct pipecmd *pcmd;
    struct redircmd *rcmd;

    if (cmd == 0)
        exit(0);

    switch (cmd->type) {
        default:
            fprintf(stderr, "Unknown command type\n");
            exit(-1);

        case ' ':
            ecmd = (struct execcmd *)cmd;
            if (ecmd->argv[0] == 0)
                exit(0);
            handle_simple_cmd(ecmd);
            break;

        case '>':
        case '<':
            rcmd = (struct redircmd *)cmd;
            handle_redirection(rcmd);
            runcmd(rcmd->cmd);
            break;

        case '|':
            pcmd = (struct pipecmd *)cmd;
            handle_pipe(pcmd, p, r);
            break;
    }
    exit(0);
}

int fork1(void) {
    /* Task 1: Implement the fork1 function.
    The function is supposed to create a new process using the `fork()` system call.
    It should print a message if the fork fails, otherwise return the process ID of the child process (or -1 if the fork fails).
    */
    pid_t pid = fork();
    if (pid < 0) {
        fprintf(stderr, "Fork function not implemented\n");
        exit(-1);
    }
    return pid;
    /* END OF TASK 1 */
}

void handle_simple_cmd(struct execcmd *ecmd) {
    /* Task 2: Implement the code below to execute simple commands. */
    execvp(ecmd->argv[0], ecmd->argv);
    fprintf(stderr, "exec not implemented\n");
    exit(-1);
    /* END OF TASK 2 */
}

void handle_redirection(struct redircmd *rcmd) {
    /* Task 3: Implement the code below to handle input/output redirection. */
    close(rcmd->fd);
    int fd = open(rcmd->file, rcmd->mode, 0644);
    if (fd < 0) {
        fprintf(stderr, "redir not implemented\n");
        exit(-1);
    }
    if (fd != rcmd->fd) {
        dup2(fd, rcmd->fd);
        close(fd);
    }
    /* END OF TASK 3 */
}

void handle_pipe(struct pipecmd *pcmd, int *p, int r) {
    /* Task 4: Implement the code below to handle pipes. */
    (void)r;

    if (pipe(p) < 0) {
        fprintf(stderr, "pipe not implemented\n");
        exit(-1);
    }

    if (fork1() == 0) {
        close(1);
        dup2(p[1], 1);
        close(p[0]);
        close(p[1]);
        runcmd(pcmd->left);
    }

    if (fork1() == 0) {
        close(0);
        dup2(p[0], 0);
        close(p[0]);
        close(p[1]);
        runcmd(pcmd->right);
    }

    close(p[0]);
    close(p[1]);
    wait(0);
    wait(0);
    /* END OF TASK 4 */
}

/* Task 5: implement cd, exit and export as built-ins.
 *
 * These three commands need to run in the shell's OWN process (not in
 * a fork), because they change the shell's state (current directory,
 * environment variables) or terminate the shell itself -- if you ran
 * them via fork+exec like a normal command, the change would happen
 * only in the child process and be lost as soon as it terminated.
 *
 * Recognize ecmd->argv[0] and implement:
 *   cd [dir]      -- chdir() to dir, or to $HOME if no argument is
 *                     given. On error (invalid directory), print a
 *                     message and keep the shell running (do not
 *                     exit).
 *   exit [code]   -- terminate the shell with the given exit code
 *                     (0 if omitted).
 *   export VAR=val -- setenv() so that VAR is visible to commands
 *                     executed afterwards (not just an internal
 *                     shell variable).
 *
 * Return 1 if argv[0] was one of these built-ins (and you already
 * handled it); return 0 otherwise, so that main() knows to fall back
 * to the normal fork1()+runcmd() path.
 *
 * This function is only consulted for a single simple command line
 * (no pipe, no redirection) -- see how it is called in main() below.
 */
int handle_builtin(struct execcmd *ecmd) {
    if (ecmd->argv[0] == 0) return 0;

    // Comando 'cd'
    if (strcmp(ecmd->argv[0], "cd") == 0) {
        char *path = ecmd->argv[1];
        if (!path) path = getenv("HOME");
        
        if (chdir(path) < 0) {
            fprintf(stderr, "cd: cannot change directory to %s\n", path);
        }
        return 1;
    }

    // Comando 'exit'
    if (strcmp(ecmd->argv[0], "exit") == 0) {
        int code = 0;
        if (ecmd->argv[1]) {
            code = atoi(ecmd->argv[1]);
        }
        exit(code);
    }

    // Comando 'export'
    if (strcmp(ecmd->argv[0], "export") == 0) {
        if (ecmd->argv[1]) {
            char *eq = strchr(ecmd->argv[1], '=');
            if (eq) {
                *eq = '\0'; // Divide a string no '='
                char *val = eq + 1;
                setenv(ecmd->argv[1], val, 1);
            }
        }
        return 1;
    }

    return 0; // Não é um comando built-in
}

/* Task 6: expand environment variables ($VAR) inside a token.
 *
 * Recognize a '$' followed by a variable name (letters, digits and
 * '_', starting with a letter or '_') anywhere inside `token`, and
 * replace that occurrence with the value of getenv() for that name.
 * If the variable is not set, replace it with the empty string --
 * never leave the literal "$VAR" in the output and never crash.
 *
 * Example: if the environment variable SAUDACAO is "Sistemas
 * Operacionais", then expand_vars("$SAUDACAO!") should return
 * "Sistemas Operacionais!".
 *
 * This is called from the line processor (see parseexec() and
 * parseredirs() below), right after mkcopy() extracts each token
 *
 * For now, this function just returns the token unchanged -- that is
 * why $VAR currently shows up literally in the output instead of
 * being substituted.
 */
char *expand_vars(char *token) {
    if (!token) return token;

    char *dollar = strchr(token, '$');
    if (!dollar) return token; // Nenhuma variável para expandir

    // Aloca um buffer seguro para o novo token expandido
    char *new_token = malloc(1024);
    memset(new_token, 0, 1024);

    char *src = token;
    char *dst = new_token;

    while (*src) {
        if (*src == '$') {
            src++;
            char var_name[256];
            int i = 0;
            
            // Lê o nome da variável (letras, números ou underscore)
            while (*src && (isalnum(*src) || *src == '_') && i < 255) {
                var_name[i++] = *src++;
            }
            var_name[i] = '\0';

            char *val = getenv(var_name);
            if (val) {
                strcpy(dst, val);
                dst += strlen(val);
            }
        } else {
            *dst++ = *src++;
        }
    }
    *dst = '\0';
    
    free(token); // Libera o ponteiro original alocado pelo mkcopy
    return new_token;   
}
int getcmd(char *buf, int nbuf) {
    if (isatty(fileno(stdin)))
        fprintf(stdout, "$ ");
    memset(buf, 0, nbuf);
    fgets(buf, nbuf, stdin);
    if (buf[0] == 0)  // EOF
        return -1;
    return 0;
}

int main(void) {
    static char buf[100];
    int r;

    // Read and execute commands.
    while (getcmd(buf, sizeof(buf)) >= 0) {
        struct cmd *cmd = parsecmd(buf);

        /* If the whole line is a single simple command (no pipe, no
         * redirection) and it names a built-in, handle it right here
         * in the shell's own process -- do not fork. */
        if (cmd->type == ' ' && handle_builtin((struct execcmd *)cmd))
            continue;

        if (fork1() == 0)
            runcmd(cmd);
        wait(&r);
    }
    exit(0);
}

/****************************************************************
 * Helper functions for creating command structures
 ***************************************************************/

struct cmd *
execcmd(void) {
    struct execcmd *cmd;

    cmd = malloc(sizeof(*cmd));
    memset(cmd, 0, sizeof(*cmd));
    cmd->type = ' ';
    return (struct cmd *)cmd;
}

struct cmd *
redircmd(struct cmd *subcmd, char *file, int type) {
    struct redircmd *cmd;

    cmd = malloc(sizeof(*cmd));
    memset(cmd, 0, sizeof(*cmd));
    cmd->type = type;
    cmd->cmd = subcmd;
    cmd->file = file;
    cmd->mode = (type == '<') ? O_RDONLY : O_WRONLY | O_CREAT | O_TRUNC;
    cmd->fd = (type == '<') ? 0 : 1;
    return (struct cmd *)cmd;
}

struct cmd *
pipecmd(struct cmd *left, struct cmd *right) {
    struct pipecmd *cmd;

    cmd = malloc(sizeof(*cmd));
    memset(cmd, 0, sizeof(*cmd));
    cmd->type = '|';
    cmd->left = left;
    cmd->right = right;
    return (struct cmd *)cmd;
}

/****************************************************************
 * Command Line Processing
 ***************************************************************/

char whitespace[] = " \t\r\n\v";
char symbols[] = "<|>";

int gettoken(char **ps, char *es, char **q, char **eq) {
    char *s;
    int ret;

    s = *ps;
    while (s < es && strchr(whitespace, *s))
        s++;
    if (q)
        *q = s;
    ret = *s;
    switch (*s) {
        case 0:
            break;
        case '|':
        case '<':
            s++;
            break;
        case '>':
            s++;
            break;
        default:
            ret = 'a';
            while (s < es && !strchr(whitespace, *s) && !strchr(symbols, *s))
                s++;
            break;
    }
    if (eq)
        *eq = s;

    while (s < es && strchr(whitespace, *s))
        s++;
    *ps = s;
    return ret;
}

int peek(char **ps, char *es, char *toks) {
    char *s = *ps;
    while (s < es && strchr(whitespace, *s)) s++;
    *ps = s;
    return *s && strchr(toks, *s);
}

struct cmd *parseline(char **, char *);
struct cmd *parsepipe(char **, char *);
struct cmd *parseexec(char **, char *);

/* Copy characters from the input buffer, starting from s to es.
 * Place a null terminator at the end to create a valid string. */
char *mkcopy(char *s, char *es) {
    int n = es - s;
    char *c = malloc(n + 1);
    assert(c);
    strncpy(c, s, n);
    c[n] = 0;
    return c;
}

struct cmd *
parsecmd(char *s) {
    char *es;
    struct cmd *cmd;

    es = s + strlen(s);
    cmd = parseline(&s, es);
    peek(&s, es, "");
    if (s != es) {
        fprintf(stderr, "leftovers: %s\n", s);
        exit(-1);
    }
    return cmd;
}

struct cmd *
parseline(char **ps, char *es) {
    struct cmd *cmd;
    cmd = parsepipe(ps, es);
    return cmd;
}

struct cmd *
parsepipe(char **ps, char *es) {
    struct cmd *cmd;

    cmd = parseexec(ps, es);
    if (peek(ps, es, "|")) {
        gettoken(ps, es, 0, 0);
        cmd = pipecmd(cmd, parsepipe(ps, es));
    }
    return cmd;
}

struct cmd *
parseredirs(struct cmd *cmd, char **ps, char *es) {
    int tok;
    char *q, *eq;

    while (peek(ps, es, "<>")) {
        tok = gettoken(ps, es, 0, 0);
        if (gettoken(ps, es, &q, &eq) != 'a') {
            fprintf(stderr, "missing file for redirection\n");
            exit(-1);
        }
        switch (tok) {
            case '<':
                cmd = redircmd(cmd, expand_vars(mkcopy(q, eq)), '<');
                break;
            case '>':
                cmd = redircmd(cmd, expand_vars(mkcopy(q, eq)), '>');
                break;
        }
    }
    return cmd;
}

struct cmd *
parseexec(char **ps, char *es) {
    char *q, *eq;
    int tok, argc;
    struct execcmd *cmd;
    struct cmd *ret;

    ret = execcmd();
    cmd = (struct execcmd *)ret;

    argc = 0;
    ret = parseredirs(ret, ps, es);
    while (!peek(ps, es, "|")) {
        if ((tok = gettoken(ps, es, &q, &eq)) == 0)
            break;
        if (tok != 'a') {
            fprintf(stderr, "syntax error\n");
            exit(-1);
        }
        cmd->argv[argc] = expand_vars(mkcopy(q, eq));
        argc++;
        if (argc >= MAXARGS) {
            fprintf(stderr, "too many args\n");
            exit(-1);
        }
        ret = parseredirs(ret, ps, es);
    }
    cmd->argv[argc] = 0;
    return ret;
}
