#include <extendp.h>

extern char     *find_and_replace(const char*, char*, int*); 

FILE             *tty;
extern int      errno;
char            *tmpfifo_n;
int             line;
char            *menu_cfg;
int             no_of_matches;
int             offset = 1;

void            insert_substring(char*, char*, int);
void            insert_substring_off(char*, char*, int);
char*           substring(char*, int, int);

void usage(FILE *tty)
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
    editor_flag = 0,
    pager_flag  = 0,
    tee_flag    = 0;

/* --------------------------------------------------------------------------*/


/* Fgets with readline */
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

void mktmpfifo()
{
    tmpfifo_n = "temp.fifo";
    if (mkfifo(tmpfifo_n, S_IREAD | S_IWRITE) != 0) {
        perror("Error: Could not create fifo ");
        exit(EXIT_FAILURE);
    }
}


/* Read in stdin, return a pointer to an array of lines */
char ** echo_in(FILE *tty, int editor_flag, int menu_flag)
{
    static char         *lines[MAXLINES];
    char                *curline = 0;
    int                 line = 0;
    int                 *no_of_matches;
    size_t              len = 0;
    ssize_t             read;
    FILE                *tmpf;

    char                *matches[100];
    char                *default_bol = color(CYAN, "-->");

    no_of_matches = 0;

    while ((read = getline(&curline, &len, stdin)) > 0) {
        lines[line] = malloc(sizeof(curline));

        if (lines[line] == 0) {
            perror("Error: Unable to allocated memory\n");
            exit(EXIT_FAILURE);
        }

        if ((lines[line] = strdup(curline)) == NULL){
            perror("Error: strdup - insufficient memory\n");
            exit(EXIT_FAILURE);
        }


        if (menu_flag == 1){   /* Select mode: number the lines */
            char *pre;
            if (!menu_cfg) {
                fprintf(tty, color(MARKERC, MENU_L) " %s", line, lines[line]);
            }
            else {
                fprintf(tty, "%s\n", find_and_replace(lines[line], matches,
                            no_of_matches));
                /*free()*/
            }
        }
        else if (editor_flag == 1)
            fprintf(tty, "%s", lines[line]);
        else
            fprintf(tty, "%s %s", default_bol, lines[line]);

        if (line++ > MAXLINES) {            /* TODO: Resizing array? */
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
    char             **lines;
    char             *prompt = (char *)NULL;
    int              opt;

    /* Write directly to tty to avoid piping the output forward */
    if ((tty = fopen(ctermid(NULL), "w+")) == NULL)
        perror("Error: fopen error");
    setbuf(tty, NULL);


    while ((opt = getopt_long(argc, argv, "mepth", long_options, NULL)) != -1) {
        switch (opt) {
            case 'm':
                menu_flag = 1;
                if (optarg)
                     menu_cfg = optarg;
                break;
            case 'e':
                editor_flag = 1;
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

    if (pager_flag) {
        FILE *pager = pager_p();
        lines = echo_in(pager, editor_flag, menu_flag );
        pclose(pager);
    }
    else if (editor_flag) {
        char        com[80];
        char        template[] = "extendp.XXXXXX";
        int         tempfd;
        FILE        *tmpf;

        if ((tempfd = mkstemp(template)) == -1)
            perror("Error: Could not open temp file");


        tmpf = fdopen(tempfd, "w+");
        echo_in(tmpf, editor_flag, menu_flag);

        if (sprintf(com, "%s %s", "vim", template) < 0)
            perror("Error: sprintf");
        printf("%s", com);
        FILE *edit = popen(com, "w");
        pclose(edit);
        int i = 0;
        while ((fgets(lines[i++], 1000, tmpf)) != NULL) {}
        
    }
    else if (menu_cfg) {
        re = get_remenu(menu_cfg);
        lines = echo_in(tty, editor_flag, menu_flag );
    }
    else
        lines = echo_in(tty, editor_flag, menu_flag );

    int choice = 0;

    /* Prompt for pipe extension */

    if (menu_flag) {
        if (menu_cfg) {
            /*printf("Regex: %s", menu_cfg);*/
        }
        fputs(MENU_PS2, tty);
        fputs(MENU_PS1, tty);
        if (fscanf(tty, "%d", &choice) != 1) {
            perror("Error: choice not recognized");
            exit(EXIT_FAILURE);
        }
        fputs(lines[choice], stdout);
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
    g_regex_unref(re);

    exit(EXIT_SUCCESS);
}


