#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <ctype.h>
#define C_MAX 2047
#define C_MIN -2048
#define SPACES " \t\f\r\v"
#define after_sppace(s) while(isspace((*s)))(s++)
static int my_strtol(char * number_candidate, long * result,char **endptr_space,int max, int min){
    errno = 0;
    *result = strtol(number_candidate,endptr_space,10);
    if(errno == ERANGE || *result > max || *result < min)
        return -1;
    if(*endptr_space == number_candidate)
        return -2;
    after_sppace(*endptr_space);
    return 0;
}


int main() {
    long g;
    char *end;
    g = my_strtol("     +",&g,&end,C_MAX,C_MIN);
    return 0;
}
