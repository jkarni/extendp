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
#include <readline/readline.h>
#include <readline/history.h>
#include <getopt.h>
#include <glib.h>

#define MAXCHAR         1000
#define MAXLINES        10000

#define PAGER           "${PAGER:-less}"
#define EDITOR          "${EDITOR:-vi}"

#define PROMPT          "xp % "
#define SPROMPT         "xp - SELECT % "
 
#define CFG  "extendp.cfg"


#define RED     "\x1b[31m"
#define GREEN   "\x1b[32m"
#define YELLOW  "\x1b[33m"
#define BLUE    "\x1b[34m"
#define MAGENTA "\x1b[35m"
#define CYAN    "\x1b[36m"
#define BOLD    "\x1b[1m"
#define RESET   "\x1b[0m"

#define color(color, text)       color "" text "" RESET 

#define NOL     3
#define OFFSET  19

#define LCYAN   4
#define LBOLD   3
#define LRESET  3

FILE             *tty;
extern int      errno;
char            *tmpfifo_n;
int             line;
char            *menu_cfg;
GRegex          *re;
int             no_of_matches;

void insert_substring(char*, char*, int);
char* substring(char*, int, int);

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

/* Config -------------------------------------------------------------------*/


/* Get the regular expression associated with a group */
GRegex * get_remenu(const gchar *group_name)
{
    const gchar    *regex;
    GKeyFile       *cfgfile;
    
    cfgfile = g_key_file_new();

    if (g_key_file_load_from_file(cfgfile, CFG, G_KEY_FILE_NONE, NULL) == FALSE) {
        perror("Error: Config file could not be loaded");
        exit(EXIT_FAILURE);
    }

    if ((regex = g_key_file_get_value(cfgfile, group_name, "regex", NULL)) == NULL) {
        perror("Error: Could not find regex for group");
        exit(EXIT_FAILURE);
    }

    printf("%s\n", regex);

    g_key_file_free(cfgfile);

    return g_regex_new("(staff)", 0, 0, NULL);

}

/* Substitute matches in a line with menu numbers and color codes, returning
 * the new line, and update the array of all matches */

char *find_and_replace(const gchar *line, char *all_matches[] ) {
    GMatchInfo   *match_info;
    int          offset = 0;
    char         number[OFFSET];
    char         *newline;
    char         digits[17];
    gint         *start;
    gint         *end;

    if ((newline = malloc(strlen(line) + 30)) == NULL) {
        perror("Error: malloc");
        exit(EXIT_FAILURE);
    }
    strcpy(newline, line);
    g_regex_match(re, line, 0, &match_info);

    while (g_match_info_matches(match_info)) {
        gchar *word = g_match_info_fetch(match_info, 0);
        all_matches[no_of_matches++] = word;
        /*sprintf(number, "%3d", no_of_matches);*/
        if (g_match_info_fetch_pos(match_info, 1, start, end)) {
            sprintf(digits, CYAN "(%3d)" RESET, no_of_matches);
            printf("%s", digits);
            insert_substring(newline, digits, 1);
        }
        /*printf("%s", menu_no(1));*/
        /*offset += OFFSET;*/
        /*insert_substring(newline, RESET, *end + offset);*/
        /*offset += LRESET;*/
        g_free(word);
        g_match_info_next(match_info, NULL);
    }
    g_match_info_free(match_info);

    return newline;
}

void insert_substring(char *a, char *b, int position)
{
   char *f, *e;
   int length;
 
   length = strlen(a);
 
   f = substring(a, 1, position - 1 );      
   e = substring(a, position, length-position+1);
 
   strcpy(a, "");
   strcat(a, f);
   free(f);
   strcat(a, b);
   strcat(a, e);
   free(e);
}
 
char *substring(char *string, int position, int length) {
   char *pointer;
   int c;
 
   pointer = malloc(length+1);
 
   if (pointer == NULL)
       exit(EXIT_FAILURE);
 
   for (c = 0 ; c < length ; c++) 
      *(pointer+c) = *((string+position-1)+c);       
 
   *(pointer+c) = '\0';
 
   return pointer;
}

/* --------------------------------------------------------------------------*/


/* Fgets with readline */
char *fgets_rl(FILE *fp)
{
    char            *buffer = (char *)NULL;
    static char     *col_prompt = color(CYAN, PROMPT);

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

/* Open pager */
FILE *pager_p()
{
    FILE                *fpager;

    if ( (fpager = popen(PAGER, "w")) == NULL)
        perror("Error: popen error");
    return fpager;
}

FILE *editor_p()
{
    FILE                *feditor;

    if ( (feditor = popen(EDITOR, "w")) == NULL)
        perror("Error: popen error");
    return feditor;

}

/* Read in stdin, return a pointer to an array of lines */
char ** echo_in(FILE *tty, int editor_flag, int menu_flag)
{
    static char         *lines[MAXLINES];
    char                *curline = 0;
    int                 line = 0;
    size_t              len = 0;
    ssize_t             read;
    FILE                *tmpf;

    char                *matches[100];

    char                *default_bol = color(CYAN, "-->");

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
                fprintf(tty, color(CYAN, "%3d)") " %s", line, lines[line]);
            }
            else {
                fprintf(tty, "%s\n", find_and_replace(lines[line], matches));
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
        fputs(SPROMPT, tty);
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


