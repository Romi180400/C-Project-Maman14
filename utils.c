#include "utils.h"
#include <stdio.h>
#define STD_RESET "\x1b[m"
#define COLOR_RED "\x1b[31m"

void asm_prnt_err(const char * file_name,const int line_num,const char *format,...) {
    va_list ptr;
    va_start(ptr,format);
    line_num > 0 ? printf("%s:%d: ",file_name,line_num) : printf("%s: ",file_name);
    printf(COLOR_RED "error: " STD_RESET);
    vprintf(format,ptr);
    putc('\n',stdout);
    va_end(ptr);

}
