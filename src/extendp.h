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

#ifdef HAVE_LIBREADLINE
#  if defined(HAVE_READLINE_READLINE_H)
#    include <readline/readline.h>
#  elif defined(HAVE_READLINE_H)
#    include <readline.h>
#  else /* !defined(HAVE_READLINE_H) */
extern char *readline ();
#  endif /* !defined(HAVE_READLINE_H) */
char *cmdline = NULL;
#else /* !defined(HAVE_READLINE_READLINE_H) */
  /* no readline */
#endif /* HAVE_LIBREADLINE */

#ifdef HAVE_READLINE_HISTORY
#  if defined(HAVE_READLINE_HISTORY_H)
#    include <readline/history.h>
#  elif defined(HAVE_HISTORY_H)
#    include <history.h>
#  else /* !defined(HAVE_HISTORY_H) */
extern void add_history ();
extern int write_history ();
extern int read_history ();
#  endif /* defined(HAVE_READLINE_HISTORY_H) */
  /* no history */
#endif /* HAVE_READLINE_HISTORY */

#include <getopt.h>
#include <glib.h>

#define MAXCHAR         1000
#define MAXLINES        10000
#define MAXCHOICES      20

#define PAGER           "${PAGER:-less}"
#define EDITOR          "${EDITOR:-vi}"



#define CFG  "/.extendp.cfg"


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

#define MARKERC   CYAN
#define MATCHC    BOLD
#define PROMPTC   CYAN

#define PS1              "✕ |"
#define MENU_PS1         "✕ |"
#define MENU_PS2         "Please enter a number, or multiple numbers separated " \
                         "by commas. \n"

#define MENU_L          "§%-3d↦"
#define MENU_R          "↤%3d§"

void * xmalloc(size_t);
void * xrealloc(void *, size_t);

extern GRegex   *re;
