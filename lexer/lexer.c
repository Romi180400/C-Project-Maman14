#include "lexer.h"
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#define after_sppace(s) while(isspace(*s))s++
/**
 * @brief
 * @param name - name
 * @param args_allow - L - Label D - number D^ - S - String
 */
struct directive_definition  {
    enum asm_dir_type adt;
    const char * name;
    const char * args_allow;
};

struct asm_defintion {
    enum asm_op_type;
    const char * name;
    const char * args_allow_src;
    const char * args_allow_dest;
};
/**
 * @brief
 * @param args_allow - L - label D - number(this is immd both symbol and numeric) R - register I - index with symbol
 */
static struct asm_defintion asm_table[16] = {
        {ast_mov,"mov","LDRI", "LRI"},
        {ast_cmp,"cmp","LDRI","LDRI"},
};
static struct directive_definition dir_table[4] = {
        {dir_external,".extern","L"},
        {dir_entry,".entry","L"},
        {dir_data,".data","D^"},
        {dir_string,".string","S"}
};

struct parse_args_result {
    char arg_syntax_error[100];
    enum {
        arg_immed,
        arg_immed_symbol,
        arg_symbol,
        arg_array_index,
        arg_register,
        arg_string
    }type;
    union {
        char *symbol;
        char * string;
        long immed;
        long reg;
        struct{
            char * symbol;
            int index_num_or_symbol;
            int index;
            char * symbol;
        }array_index;
    }result;
};

static int my_strtol(char * number_candidate, long * result);

static int is_valid_symbol(char *symbol_candidate,char ** symbol_ret);

static struct parse_args_result parse_args(char * args_string,const char * args_allow);

static void parse_asm_arg(struct parse_args_result * par,int src_or_dest, struct ast * ast) {
    switch (par->type)
    {
        case arg_immed:
            ast->ast_options.ast_op.operands[src_or_dest].operand_option = operand_immd;
            ast->ast_options.ast_op.operands[src_or_dest].operand.immd = par->result.immed;
            break;
        case arg_immed_symbol:
            ast->ast_options.ast_op.operands[src_or_dest].operand_option = operand_immd_symbol;
            ast->ast_options.ast_op.operands[src_or_dest].operand.symbol = par->result.symbol;
            break;
        case arg_array_index:
            ast->ast_options.ast_op.operands[src_or_dest].operand_option = operand_array_index;
            if(par->result.array_index.index_num_or_symbol == array_index_label) {
                ast->ast_options.ast_op.operands[src_or_dest].operand.array_index.index_pair.label = par->result.array_index.symbol;
            }else {
                ast->ast_options.ast_op.operands[src_or_dest].operand.array_index.index_pair.index = par->result.array_index.index;
            }
            break;
        case arg_symbol:
            ast->ast_options.ast_op.operands[src_or_dest].operand.symbol = par->result.symbol;
            break;
        case arg_register:
            ast->ast_options.ast_op.operands[src_or_dest].operand.reg = par->result.reg;
            break;
        default:
            break;
    }
}
static void parse_asm_args(char *args_string,struct asm_defintion *asm_def,struct ast * ast) {
    struct parse_args_result par = {0};
    char * separator;
    char *src, *dest;
    after_sppace(args_string);
    separator = strchr(arg_string,',');
    if(!separator  && asm_def->args_allow_src) {
        sprintf(ast->syntax_error,"missing ',' separator for instruction:'%s'",asm_def->name);
        return;
    }
    if(separator) {
        src = args_string;
        dest = separator +1;
        separator = '\0';
        par = parse_args(src,asm_def->args_allow_src);
        if(par.arg_syntax_error[0] != '\0') {
            sprintf("error in source argument of instruction:'%s' : %s",asm_def->name,par.arg_syntax_error);
            return;
        }
        parse_asm_arg(&par,0,ast);

        par = parse_args(dest,asm_def->args_allow_dest);
        if(par.arg_syntax_error[0] != '\0') {
            sprintf("error in destination argument of instruction:'%s' : %s",asm_def->name,par.arg_syntax_error);
            return;
        }
        parse_asm_arg(&par,1,ast);
    }else {
        if(asm_def->args_allow_dest) {
            par = parse_args(dest,asm_def->args_allow_dest);
            if(par.arg_syntax_error[0] != '\0') {
                sprintf("error in destination argument of instruction:'%s' : %s",asm_def->name,par.arg_syntax_error);
                return;
            }
            parse_asm_arg(&par,1,ast);
        }
        else {
            if(*args_string != '\0') {
                sprintf("instruction :'%s' expects no arguments but have:'%s'",asm_def->name,args_string);
                return;
            }
        }
    }
}
static void parse_dir_args(char *args_string, struct directive_definition * asm_dir_def,struct ast * ast)  {
    switch(asm_dir_def->adt) {
        case dir_entry: case dir_external:

            break;
        case dir_data:
            break;
        case dir_string:
            break;
    }

}
struct ast lexer_get_ast(const char * line) {
    struct ast new_ast = {0};
    /* algo goes here...*/


    return new_ast;
}