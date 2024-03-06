#include "lexer.h"
#include <stdio.h>


int main() {
    struct ast n_ast;
    FILE * test_f;
    int line_c = 1;
    char buffer[82];
    test_f = fopen("/home/yarin/work/romi_omer/C-Project-Maman14/lexer/test.as","r");
    if(test_f) {
        while(fgets(buffer,sizeof(buffer),test_f)) {
            n_ast = lexer_get_ast(buffer);
        }
        line_c++;
    }
    return 0;
}