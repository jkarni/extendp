#include "extendp.h"

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
