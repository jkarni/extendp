#include "extendp.h"

/* extendp
 *
 * Print out and manipulate interactively stdin before passing the result to
 * stdout. 
 */

extern void find_and_replace(const char*, char**, char* );

int             cursize = MAXLINES;

FILE             *tty;
int             line;
char            *menu_cfg;
int             counta = 0;

GRegex          *get_remenu(gchar*);
void            insert_substring(char*, char*, int);
void            insert_substring_off(char*, char*, int);
char*           substring(char*, int, int);
char            **matches;


static void usage(FILE *tty)
{
    fputs("Usage: ", tty);
    fputs("xp [-seSE]\n", tty);
    exit(EXIT_FAILURE);
}


/* Options ------------------------------------------------------------------*/

struct option long_options[] =
{
    {"menu",      optional_argument,      0,      'm'},
    {"editor",    no_argument,            0,      'e'},
    {"pager",     no_argument,            0,      'p'},
    {"tee",       no_argument,            0,      't'},
    {"help",      no_argument,            0,      'h'},
    {0,           0,                      0,       0 }
};

int menu_flag   = 0,
    pager_flag  = 0,
    tee_flag    = 0;



/* --------------------------------------------------------------------------*/




/* Fgets with GNU readline */
char *fgets_rl(FILE *fp)
{
    char            *buffer = (char *)NULL;
    static char     *col_prompt = color(CYAN, PS1);

    rl_instream = fp;
    rl_outstream = fp;
    buffer = readline(col_prompt);

    /* If the line has any text in it,
       save it on the history. */
    if (buffer && *buffer)
        add_history (buffer);

    return buffer;
}

FILE *pager_p()
{
    FILE *output;

    char *pager = getenv("PAGER") ? getenv("PAGER") : "less";

    output = popen(pager, "w");
    if (!output) {
       perror("Error: popen error.\n");
       exit(EXIT_FAILURE);
    }
    return output;
}

/* Read in stdin, and store input a pointer to an array of lines 
 *
 * Params:
 *      *tty: FILE to read from
 *      *result[]: pointer to array in which read lines will be stored
 *
 * Returns:
 *      void
 *
 * */
static char **echo_in(FILE *tty, char **result, int menu_flag)
{
    char                *curline = 0;
    int                 line = 0;
    size_t              len = 0;
    extern char         **matches;
    extern int          cursize;
    char                **new_result = NULL;

    
    char                *re_line = NULL;


    while (getline(&curline, &len, stdin) > 0) {
        result[line] = malloc(sizeof(curline));

        if (result[line] == 0) {
            perror("Error: Unable to allocated memory\n");
            exit(EXIT_FAILURE);
        }

        if ((result[line] = strdup(curline)) == NULL){
            perror("Error: strdup - insufficient memory\n");
            exit(EXIT_FAILURE);
        }


        if (menu_flag == 1){   /* Select mode: number the lines */
            if (!menu_cfg) {
                fprintf(tty, color(MARKERC, MENU_L) " %s", line + 1, result[line]);
            }
            else {
                if ((re_line = malloc(strlen(result[line]) + 1000)) == NULL) {
                    perror("Error: malloc");
                    exit(EXIT_FAILURE);
                }
                find_and_replace(result[line], matches, re_line);
                fprintf(tty, "%s", re_line);
                free(re_line);
            }
        }
        else
            fprintf(tty, "%s %s", color(PROMPTC, PS1), result[line]);

        if (line++ == cursize) {
            cursize = 2*cursize;
            if ((new_result = (char **) realloc(result, cursize*sizeof(char*))) == NULL) {
                perror("Error reallocating memory");
                free(result);
                exit(EXIT_FAILURE);
            }
            else {
                result = new_result;
            }
        }
    }
    fputs("\n", tty);
    free(curline);

    result[line] = NULL;
    return result;
}

int main(int argc, char *argv[])
{
    FILE             *fpout;   /* Pipe extension */
    char             **lines = NULL;
    char             *prompt = (char *)NULL;
    int              opt;
    extern char      *optarg;
    extern GRegex    *re;

    if ((lines = (char**)malloc(sizeof(char*) * MAXLINES)) == NULL) {
        perror("Error: fopen error");
        exit(EXIT_FAILURE);
    }
   

    /* Write directly to tty to avoid piping the output forward */
    if ((tty = fopen(ctermid(NULL), "w+")) == NULL) {
        perror("Error: fopen error");
        exit(EXIT_FAILURE);
    }
    setbuf(tty, NULL);


    while ((opt = getopt_long(argc, argv, "mepth", long_options, NULL)) != -1) {
        switch (opt) {
            case 'm':
                menu_flag = 1;
                if (optarg)
                     menu_cfg = optarg;
                break;
            case 'p':
                pager_flag = 1;
                break;
            case 't':
                tee_flag = 1;
                break;
            case 'h':
                usage(tty);
                break;
            default:
                fputs("extendp: ", tty);
                fputs("Unrecognized argument.\n", tty);
                usage(tty);
                break;
        }
    }



    if (menu_cfg) {
        matches = malloc(1000*sizeof(char *));
        re = get_remenu(menu_cfg);
        if (!pager_flag)
            lines = echo_in(tty, lines, menu_flag );
        else {
            FILE *pager = pager_p();
            lines = echo_in(pager, lines, menu_flag );
            pclose(pager);
        }
        g_regex_unref(re);
    }
    else {
        if (!pager_flag)
            lines = echo_in(tty, lines, menu_flag );
        else {
            FILE *pager = pager_p();
            lines = echo_in(pager, lines, menu_flag );
            pclose(pager);
        }
    
    }


    /* Prompt for pipe extension */

    if (menu_flag) {
        int choice[MAXCHOICES] = {0};
        int no_choices = 0;

        fputs(color(PROMPTC, MENU_PS2), tty);
        fputs(color(PROMPTC, MENU_PS1), tty);

        if (fflush(tty) !=0 ) {
            perror("Error: choice not recognized");
            exit(EXIT_FAILURE);
        }


        fscanf(tty, "%4d", &choice[no_choices++]);
        while (no_choices < MAXCHOICES && 
                fscanf(tty, ",%4d", &choice[no_choices++]) == 1) { }
         
        no_choices = 0;
        while (choice[no_choices] != 0) {
            if (!menu_cfg)
                fputs(lines[choice[no_choices++] - 1], stdout);
            else {
                fputs(matches[choice[no_choices++] - 1], stdout);
                fputs("\n", stdout);
            }
        }
        if (menu_cfg)
            free(matches);
             
        
        exit(EXIT_SUCCESS);
    }
    else
        prompt = fgets_rl(tty);

    if (prompt != NULL && strlen(prompt) == 0) {
        /* No pipe extensions given, return stdin */
        int j = 0;
        while (lines[j] != NULL) {
            if (fputs(lines[j++], stdout) == EOF)
                perror("Error: fputs to stdout");
        }
    }


    if (!tee_flag) {
        /* Open pipe to pipe extension */
        if ((fpout = popen(prompt, "w")) == NULL)
            perror("Error: Could not open pipe");
    }
    else {
        char *ttyn = "/dev/tty";
        char com[MAXCHAR];
        snprintf(com, MAXCHAR, "%s > %s", prompt, ttyn);
        if ((fpout = popen(com, "w")) == NULL)
            perror("Error: Could not open pipe");
    }

    /* Pass stdin to pipe extension (and, if t, to stdout) */
    int i = 0;
    while (lines[++i] != NULL) {
        if (fputs(lines[i - 1], fpout) == EOF)
            perror("Error: fputs to pipe\n");
        if (tee_flag) {
            if (fputs(lines[i - 1], stdout) == EOF)
                perror("Error: fputs to stdout\n");
        }
    }
    free(prompt);
    fclose(tty);
    pclose(fpout);

    exit(EXIT_SUCCESS);
}


