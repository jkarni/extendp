#include <extendp.h>

/* readcfg.c
 *
 * Readcfg includes functions for reading the (ini-style) configuration file,
 * and processing lines with regexes for inclusiong in a extendp menu. */

GRegex          *re;
int             offset;

void            insert_substring_off(char*, char*, int);
static void     insert_substring(char*, char*, int);
static char     *substring(char*, int, int);
static GRegex   *get_remenu(const gchar*);

/* Get the regular expression associated with a group */
static GRegex * get_remenu(const gchar *group_name)
{
    const gchar    *regex;
    GKeyFile       *cfgfile;

    offset = 1;

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

    return g_regex_new("(jkarni)", 0, 0, NULL);

}

/* Substitute matches in a line with menu numbers and color codes, returning
 * the new line, and update the array of all matches */

char *find_and_replace(const gchar *line, char *all_matches[], int *no_of_matches )
{
    GMatchInfo   *match_info;
    char         number[OFFSET];
    char         *newline;
    char         digits[30];
    gint         *start = malloc(5);
    gint         *end   = malloc(5);
    int          offset = 1;

    if ((newline = malloc(strlen(line) + 40)) == NULL) {
        perror("Error: malloc");
        exit(EXIT_FAILURE);
    }
    strcpy(newline, line);
    g_regex_match(re, line, 0, &match_info);

    while (g_match_info_matches(match_info)) {

        gchar *word = g_match_info_fetch(match_info, 0);
        all_matches[(*no_of_matches)++] = word;

        if (g_match_info_fetch_pos(match_info, 1, start, end)) {

            sprintf(digits, CYAN "(%3d->)" RESET BOLD, *no_of_matches);
            insert_substring_off(newline, digits, *start);

            sprintf(digits, RESET CYAN "(<-%3d)" RESET, *no_of_matches);
            insert_substring_off(newline, digits, *end);
        }

        g_free(word);
        g_match_info_next(match_info, NULL);
    }
    g_match_info_free(match_info);

    return newline;
}

void insert_substring_off(char *a, char *b, int position) {
   insert_substring(a, b, position + offset);
   offset += strlen(b);
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

char *substring(char *string, int position, int length)
{
   char *pointer;
   int c;

   pointer = malloc(length + 1);

   if (pointer == NULL)
       exit(EXIT_FAILURE);

   for (c = 0 ; c < length ; c++)
      *(pointer + c) = *((string + position - 1) + c);

   *(pointer + c) = '\0';

   return pointer;
}

/* --------------------------------------------------------------------------*/




