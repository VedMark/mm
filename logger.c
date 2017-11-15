#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "include/logger.h"

char *get_current_time() {
    size_t size = 0;
    time_t t = 0;
    char *buf = 0;

    t = time(NULL);
    char *time_str = asctime( localtime(&t) );
    time_str[strlen(time_str) - 1] = 0;

    size = strlen(time_str) + 1 + 2;
    buf = (char*)malloc(size);
    memset(buf, 0x0, size);
    snprintf(buf,size,"[%s]", time_str);

    return buf;
}

void log_print(char *fmt, ...) {
    va_list list;
    char    *p = 0;
    char    *str_arg = 0;
    int     int_arg = 0;

    printf("%s ", get_current_time());
    va_start(list, fmt);

    for(p = fmt; *p; ++p) {
        if (*p != '%' ) {
            putchar(*p);
        }
        else {
            switch ( *++p ) {
                case 's': {
                    str_arg = va_arg( list, char * );
                    printf("%s", str_arg);
                    continue;
                }

                case 'd': {
                    int_arg = va_arg( list, int );
                    printf("%d", int_arg);
                    continue;
                }
                default: putchar(*p);
            }
        }
    }
    va_end( list );
    printf("\n");
}
