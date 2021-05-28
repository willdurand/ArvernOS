#include <string.h>  
/**
 * strpbrk - searches for one of the characters in the list you give it as a string
 * @src  :  main string  
 * @s    :  characters list 
 */ 

char *strpbrk ( const char * src , const char *s ) {
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
