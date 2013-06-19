#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <termios.h>
#include <string.h>

#define MAXCHAR 1000
#define MAXLINES 10000

#define PAGER "${PAGER:-less}"

extern int errno;

int line;

void usage(FILE *tty)
{
    fputs("Usage: ", tty);
    fputs("xp [-seSE]\n", tty);
    exit(EXIT_FAILURE);
}

char *fgets_b(char *buffer, size_t buflen, FILE *fp)
{
    if (fgets(buffer, buflen, fp) != NULL)
    {
        size_t len = strlen(buffer);
        if (len > 0 && buffer[len-1] == '\n')
            buffer[len-1] = '\0';
        return buffer;
    }
    return 0;

}


/* Send input to pager */
/*void pager_p(FILE *tty)*/
/*{*/
    /*FILE *fpager;*/
    /*if ( (fpager = fopen(PAGER, "w")) == NULL) */
        /*perror("Error: popen error")*/
/*}*/

/* Read in stdin, return a pointer to an array of lines */
char ** echo_in(FILE *tty, int S_flag)
{
    static char         *lines[MAXLINES];
    char                *curline = 0;
    int                 line = 0;
    size_t              len = 0;
    ssize_t             read;

    while ((read = getline(&curline, &len, stdin)) > 0)
    {
        lines[line] = malloc(sizeof(curline));

        if (lines[line] == 0)
        {
            perror("Error: Unable to allocated memory\n");
            exit(EXIT_FAILURE);
        }

        if ((lines[line] = strdup(curline)) == NULL){
            perror("Error: strdup - insufficient memory\n");
            exit(EXIT_FAILURE);
        }
        

        if (S_flag == 1)   /* Select mode: number the lines */
            fprintf(tty, "%3d] %s", line, lines[line]);
        else
            fprintf(tty, "> %s", lines[line]);

        if (line++ > MAXLINES) /* TODO: Resizing array? */
        {
            perror("Error: Too many lines.\n");
            exit(EXIT_FAILURE);
        }
    }
    fputs("\n", tty);

    lines[line] = NULL; 

    return lines;
}

int main(int argc, char *argv[])
{
    FILE             *fpout;   /* Pipe extension */
    FILE             *tty;
    int              master;
    char             **lines;
    char             prompt[MAXCHAR];

    /* Write directly to tty to avoid piping the output forward */
    if ((tty = fopen(ctermid(NULL), "w+")) == NULL)
        perror("Error: fopen error");
    setbuf(tty, NULL);

    /* Opts */
    int s_flag, e_flag, E_flag, S_flag;
    s_flag = e_flag = S_flag = E_flag = 0;


    while ((argc > 1) && (argv[1][0] == '-'))
    {
        switch (argv[1][1])
        {
            case 's':
                s_flag = 1;
                break;
            case 'e':
                e_flag = 1;
                break;
            case 'S':
                S_flag = 1;
                break;
            case 'E':
                E_flag = 1;
                break;
            default:
                fputs("extendp: ", tty);
                fputs("Unrecognized argument.\n", tty);
                usage(tty);
                break;
        }
        ++argv;
        --argc;
    }

    lines = echo_in(tty, S_flag );

    int choice = 0;

    /* Prompt for pipe extension */
    fputs("extendp %? ", tty);

    if (S_flag)
    {
        if (fscanf(tty, "%d", &choice) != 1)
        {
            perror("Error: choice not recognized");
            exit(EXIT_FAILURE);
        }
        fputs(lines[choice], stdout);
        exit(EXIT_SUCCESS);
    }
    else if (fgets_b(prompt, 100, tty) == NULL)
        perror("Error: Could not read user input.\n");
    else if (strlen(prompt) == 0)   /* No pipe extensions given, return stdin */
    {
        int j = 0;
        while (lines[j] != NULL)
        {
            if (fputs(lines[j++], stdout) == EOF)
                perror("Error: fputs to stdout");
        }
    }

    /* Open pipe to pipe extension */
    if ((fpout = popen(prompt, "w")) == NULL)
        perror("Error: Could not open pipe");

    /* Pass stdin to pipe extension */
    int i = 0;
    while (lines[i] != NULL)
    {
        if (fputs(lines[i++], fpout) == EOF)
            perror("Error: fputs to pipe\n");
    }
    pclose(fpout);
    exit(EXIT_SUCCESS);
}


