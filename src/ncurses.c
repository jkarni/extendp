#include <ncurses.h>
#include <form.h>
#include <stdlib.h>
#include <readline/readline.h>


#define MAXLINES 1000
#define RPAD_SIDE_MARGIN 3

WINDOW *create_newwin(int height, int width, int starty, int startx);
void destroy_win(WINDOW *local_win);

WINDOW *create_newpad(int ch, int height, int width, int starty, int startx){
    WINDOW *pad;
    FILE *f;
    pad = newpad(200, MAXLINES);
    // TODO: hook up so it reads string or stream.
    f = fopen("ncurses.c", "r");
    if (f == NULL)
        exit(EXIT_FAILURE);
    while ( (ch=fgetc(f)) != EOF)
        waddch(pad,ch);
    fclose(f);
    return pad;
}


void null() {
}

void init_colors() {
    init_pair(1,COLOR_WHITE,COLOR_BLACK);
    init_pair(2,COLOR_WHITE,COLOR_YELLOW);
}

void endp(char* msg){
    exit(EXIT_FAILURE);
}

int no_comm_win = 0 ;

void split_comm_win() {
    int i;
    no_comm_win++;
    for (i=0; i < no_comm_win; i++) {
        werase(kkk
}

int main()
{
    int x, y,
        max_x, max_y,
        rw_startx, rw_starty, rw_width, rw_height,
        sw_startx, sw_starty, sw_width, sw_height,
        ch,
        pad_line, pad_col;
    WINDOW *comm_win, *res_win;
    WINDOW *search_win, *pad;


    /* Curses initialization */
    initscr();
    noecho();
    nonl();
    raw();
    keypad(stdscr, TRUE);

    getmaxyx(stdscr, max_y, max_x);

    if(!has_colors())
        endp("Terminal does not support colors");

    start_color();
    use_default_colors();

    werase(stdscr);

    comm_win = newwin((max_y - 2) / 2, max_x - 2, 0, 0);
    wbkgdset(comm_win, COLOR_PAIR(1));
    werase(comm_win);

    res_win = newwin((max_y - 2) / 2 , max_x - 2, max_y / 2, 0);
    wbkgdset(res_win, COLOR_PAIR(1));
    werase(res_win);

    /* Readline initialization */
    rl_callback_handler_install ("»", null);



    /* Create and initialize windows */
    pad_line = pad_col = 0;
    rw_height = max_y - 6;
    rw_width  = max_x - 6;
    rw_startx = 3;
    rw_starty = 1;
    mvhline( 2, 5, 0, max_x - 10);
    pad = create_newpad(ch, rw_height, rw_width, rw_starty, rw_startx);
    attron(A_BOLD);
    mvprintw(1, (max_x / 2) - 8, "Current xp out:");
    attroff(A_BOLD);
    refresh();
    prefresh(pad, pad_line, pad_col, 4, 4, rw_height, rw_width);

    sw_height = 3;
    sw_width = rw_width;
    sw_starty = rw_height + 1;
    sw_startx = 3;
    search_win = create_newwin(sw_height, sw_width, sw_starty, sw_startx);
    box(search_win, 0, 0);
    refresh();

    wbkgd(search_win, COLOR_PAIR(2));
    wrefresh(search_win);
    wprintw(search_win, " Type commands: ");
    getyx(search_win, y, x);
    wmove(search_win, y+1, 1);
    wrefresh(search_win);


    /* Get input */
    noecho();               // Don't echo() while we do getch
    while(1) {
        ch = getch();
        rl_callback_read_char();
        switch(ch) {
            case KEY_UP:
                if (pad_line > 0) {
                    pad_line--;
                    prefresh(pad, pad_line, pad_col, 4, 4, rw_height, rw_width);
                    getyx(search_win, y, x);
                    wmove(search_win, y, x);
                    wrefresh(search_win);
                    refresh();
                }
                break;
            case KEY_DOWN:
                pad_line++;
                prefresh(pad, pad_line, pad_col, 4, 4, rw_height, rw_width);
                getyx(search_win, y, x);
                wmove(search_win, y, x);
                wrefresh(search_win);
                refresh();
                break;
            case KEY_LEFT:
                if (pad_col > 0) {
                    pad_col--;
                    prefresh(pad, pad_line, pad_col, 4, 4, rw_height, rw_width);
                    refresh();
                }
                break;
            case KEY_RIGHT:
                pad_col++;
                prefresh(pad, pad_line, pad_col, 4, 4, rw_height, rw_width);
                refresh();
                break;
            case '|':
                // TODO: Recalculate stdout
                refresh();
            case 'x':
                endwin();
                return 0;
        }
    }
    endwin();
    return 0;
}

WINDOW *create_newwin(int height, int width, int starty, int startx) {
    WINDOW *local_win;
    local_win = newwin(height, width, starty, startx);
    wrefresh(local_win);            /* Show that box */
    return local_win;
}

void destroy_win(WINDOW *local_win) {
    wborder(local_win, '|', '|', '-','-','+','+','+','+');
    wrefresh(local_win);
    delwin(local_win);
}
