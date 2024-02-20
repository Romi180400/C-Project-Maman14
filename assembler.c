#include <stdio.h>
#include "lexer.h"
#include "preproc.h"
/**
 * @brief builds the program
 *
 * @return int
 */
static int assembler_second_pass() {
    char line_buffer[100] = {0};
    FILE * in;
    struct ast AST;
    while(fgets(line_buffer,sizeof(line_buffer),in)) {
        AST = lexer_get_ast(line_buffer);

    }
}
/**
 * @brief builds the symbol table
 *
 * @return int
 */
static int assembler_first_pass() {
    char line_buffer[100] = {0};
    FILE * in;
    struct ast AST;
    while(fgets(line_buffer,sizeof(line_buffer),in)) {
        AST = lexer_get_ast(line_buffer);
    }
}

void assembler_main_routine(int file_count, char ** file_names) {
    char * am;
    int i;
    for(i=0;i<file_count;i++) {
        if( am = pre_processor(file_names[i]) ) {
            if(assembler_first_pass()) {
                if(assembler_second_pass()) {

                }
            }
        }
    }
}