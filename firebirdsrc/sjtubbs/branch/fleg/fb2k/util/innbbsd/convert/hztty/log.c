/* $Id: log.c,v 1.2 2004/06/29 14:59:22 rwzmm Exp $ */

#ifndef lint
static char *rcs_id = "$Id: log.c,v 1.2 2004/06/29 14:59:22 rwzmm Exp $";
#endif                          /* lint */

#include "config.h"
#include "io.h"

char *io_log(s, plen, inst)
char *s;
int *plen;
int inst;
{
    write(inst, s, *plen);
    return (s);
}

int log_init(arg)
char *arg;
{
    int f = open(arg, O_WRONLY | O_CREAT | O_TRUNC, 0644);

    if (f < 0)
    {
        perror(arg);
        return (-1);
    }
    return (f);
}
