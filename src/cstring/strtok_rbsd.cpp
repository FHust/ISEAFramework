#ifndef HAVE_STRTOK_R

#include "strtok_rbsd.h"

#if defined( LIBC_SCCS ) && !defined( lint )
static char *rcsid = "$OpenBSD: strtok.c,v 1.3 1999/11/09 11:19:46 art Exp $";
#endif /* LIBC_SCCS and not lint */

#include <cstring>

char *strtok_rbsd( char *s, const char *delim, char **last )
{
    register char *spanp;
    register int c, sc;
    char *tok;

    if ( s == NULL && ( s = *last ) == NULL )
        return ( NULL );

/*
 * Skip (span) leading delimiters (s += strspn(s, delim), sort of).
 */
cont:
    c = *s++;
    for ( spanp = (char *)delim; ( sc = *spanp++ ) != 0; )
    {
        if ( c == sc )
            goto cont;
    }

    if ( c == 0 )
    { /* no non-delimiter characters */
        *last = NULL;
        return ( NULL );
    }
    tok = s - 1;

    /*
     * Scan token (scan for delimiters: s += strcspn(s, delim), sort of).
     * Note that delim must have one NUL; we stop if we see that, too.
     */
    for ( ;; )
    {
        c = *s++;
        spanp = (char *)delim;
        do
        {
            if ( ( sc = *spanp++ ) == c )
            {
                if ( c == 0 )
                    s = NULL;
                else
                    s[-1] = 0;
                *last = s;
                return ( tok );
            }
        } while ( sc != 0 );
    }
    /* NOTREACHED */
}

#endif /* !HAVE_STRTOK_R */
