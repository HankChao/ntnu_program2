#include <stddef.h>
#include <stdio.h>
#include <stdint.h>
#include "mystring.h"

char *mystrchr(const char *s, int c){
    if(s == NULL){
        return NULL;
    }

    while (*s){
        if(*s == c){
            return (char *)s;
        }
        s++;
    }
    if(c == '\0')
        return (char *)s;

    return NULL;
}

char *mystrrchr(const char *s, int c){
    if(s == NULL){
        return NULL;
    }

    const char *last = NULL;
    while(*s){
        if(*s == c)
            last = s;
        s++;
    }
    if(c == '\0'){
        return (char *)s;
    } 
    return (char *)last;
}

size_t mystrspn(const char *s, const char *accept){
    if(s == NULL || accept == NULL){
        return 0;
    }
    size_t count = 0;
    while(*s && mystrchr(accept, *s)){
        count++;
        s++;
    } 
    return count;
}

size_t mystrcspn(const char *s, const char *reject){
    if(s == NULL || reject == NULL){
        return 0;
    }
    size_t count=0;
    while(*s && !mystrchr(reject,*s)){
        count++;
        s++;
    }
    return count;
}

char *mystrpbrk(const char *s, const char *accept){
    if(s == NULL || accept == NULL){
        return NULL;
    }
    while(*s){
        if(mystrchr(accept,*s))
            return (char *)s;
        s++;
    }
    return NULL;
}

char *mystrstr(const char *haystack , const char *needle){
    if(haystack == NULL || needle == NULL){
        return NULL;
    }
    if(!*needle)    return (char *)haystack;
    for(; *haystack; haystack++){
        if(*haystack == *needle){
            const char *h = haystack, *n = needle;
            while(*h && *n && *h == *n){
                h++;n++;
            }
            if(!*n) return (char *)haystack;
        }
    }
    return NULL;
}

char *mystrtok(char *str, const char *delim) {
    static char *next = NULL;
    static int strok_check = 0;

    if (delim == NULL) return NULL;

    if (str == NULL && strok_check == 0) {
        printf("Error: mystrtok() must first be called with a non-NULL string.\n");
        return NULL;
    }

    if (str) {
        next = str;
        strok_check = 1;
    }

    while (*next && mystrchr(delim, *next)) next++;

    if (!*next) return NULL; 

    char *start = next; 

    while (*next && !mystrchr(delim, *next)) next++;
    if (*next) *next++ = '\0';

    return start;
}

