#include <string.h>  

char *strpbrk ( const char * src , const char *s ) 
{
    char *_b  =  (char *) s ; 
    while ( *src != *s && *src ) 
    {
        s++ ; 
        if (!*s)
        {
            s=_b ; 
            src++; 
        }
    }
    return  (char *) src ; 
}
