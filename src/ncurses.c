#include <ncurses.h>
#include <form.h>
#include <cdk.h>
#include <stdlib.h>
#include <readline/readline.h>


#define MAXLINES 1000

int main () {
    WINDOW          *mwin;
    CDKSCREEN       *screen;
    CDKSWINDOW      *preview;
    CDKENTRY        *first_c,
                    *post_c;
    int             max_x,
                    max_y;
    int             ch;
    FILE            *f;
    char            *input_comm;

    mwin   = initscr();
    screen = initCDKScreen(mwin);

    initCDKColor();
    getmaxyx(mwin, max_y, max_x);


    refreshCDKScreen(screen);
    preview = newCDKSwindow(
            screen,
            0,                      // xpos
            0,                      // ypos
            max_y - 2,              // height
            max_x/ 2,               // width
            "<C></U/B>Preview<!U!B>",     // title
            100,                    // no of lines to save
            true,                   // box?
            false                   // shadow?
    );

    // Create an entry for the first command
    first_c = newCDKEntry(
            screen,
            max_x / 2,                  // xpos
            0,                          // ypos
            "</U/B>Command:<!U!B>",        // title
            "",                         // label
            0,                          // field attributes
            ' ',                        // filler character
            vCHAR,                      // display type
            max_x / 2,                  // field width
            0,                          // min char
            256,                          // max char
            true,                       // box?
            false                       // shadow?
    );

    // Create an entry for possible post commands
    post_c = newCDKEntry(
            screen,
            max_x / 2,                  // xpos
            max_y - 2,                  // ypos
            "</U/B>Post Command:<!U!B>",   // title
            "",                         // label
            0,                          // field attributes
            ' ',                        // filler character
            vCHAR,                      // display type
            max_x / 2,                  // field width
            0,                          // min char
            256,                        // max char
            true,                       // box?
            false                       // shadow?
    );


    refreshCDKScreen(screen);
    input_comm = activateCDKEntry(first_c, 0);

    execCDKSwindow(preview, input_comm, 0);

    activateCDKSwindow(preview, NULL);

    activateCDKEntry(post_c, 0);
    /*      Cleanup          */
    destroyCDKScreen (screen);
    endCDK();
    delwin(mwin);

    return 1;
}

