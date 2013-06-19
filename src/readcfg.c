#include "extendp.h"

/* readcfg.c
 *
 * Readcfg includes functions for reading the (ini-style) configuration file,
 * and processing lines with regexes for inclusiong in a extendp menu. */

GRegex          *re;

void            insert_substring_off(char*, char*, int);
static void     insert_substring(char*, char*, int);
static char     *substring(char*, int, int);
GRegex          *get_remenu(const gchar*);
int             no_of_matches = 0;
int             offset;

int count = 0;


/* Get the regular expression associated with a group in the cfg file 
 *
 * Params:
 *      group_name: string name of the group to be searched for in the config
 *    file, without square brackes.
 *
 * Returns:
 *      Glib regex of the group.
 * */
GRegex *get_remenu(const gchar *group_name)
{
    const gchar    *regex;
    GKeyFile       *cfgfile;
    GRegex         *ret;
    char           *home;

    home = strcat(getenv("HOME"), CFG);

    cfgfile = g_key_file_new();

    if (g_key_file_load_from_file(cfgfile, home , G_KEY_FILE_NONE, NULL) == FALSE) {
        perror("Error: Config file could not be loaded");
        exit(EXIT_FAILURE);
    }

    if ((regex = g_key_file_get_value(cfgfile, group_name, "regex", NULL)) == NULL) {
        perror("Error: Could not find regex for group");
        exit(EXIT_FAILURE);
    }

    g_key_file_free(cfgfile);

    ret = g_regex_new(regex, 1, 0, NULL);
    /*free(*regex);*/

    return ret;

}

/* Substitute matches in a line with menu numbers and color codes, returning
 * the new line, and update the array of all matches */
void find_and_replace(const gchar *line, char **all_matches, char *newline)
{
    GMatchInfo        *match_info;
    char              digits[30];
    static gint       start;
    static gint       end;
    extern int        offset;
    extern GRegex     *re;
    
    offset = 1;

    char **mpointer = all_matches;

    strcpy(newline, line);
    g_regex_match(re, line, 0, &match_info);

    while (g_match_info_matches(match_info)) {

        gchar *word = g_match_info_fetch(match_info, 1);
        if ((mpointer[no_of_matches++] = strdup(word)) == NULL) {
            perror("Error: strdup - insufficient memory\n");
            exit(EXIT_FAILURE);
        };

        if (g_match_info_fetch_pos(match_info, 1, &start, &end) && (start != -1)) {

            sprintf(digits, CYAN MENU_L RESET RED BOLD, no_of_matches);
            insert_substring_off(newline, digits, start);

            sprintf(digits, RESET CYAN  MENU_R  RESET, no_of_matches);
            insert_substring_off(newline, digits, end);
        }

        g_free(word);
        g_match_info_next(match_info, NULL);
    }
    /*free(start);*/
    /*free(end);*/
    g_match_info_free(match_info);

}
     

void insert_substring_off(char *a, char *b, int position) {
   extern int offset;

   insert_substring(a, b, (position + offset));
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




