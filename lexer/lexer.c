#include "lexer.h"
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdio.h>
#include <errno.h>
#define C_MAX 2047
#define C_MIN -2048
#define SPACES " \t\f\r\v"
#define after_sppace(s) while(isspace((*s)))(s++)
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
    enum asm_op_type aot;
    const char * name;
    const char * args_allow_src;
    const char * args_allow_dest;
};
/**
 * @brief
 * @param args_allow - L - label D - constant(this is immd both symbol and numeric) R - register I - index with symbol
 */
static struct asm_defintion asm_table[16] = {
        {ast_mov,"mov","LDRI", "LRI"},
        {ast_cmp,"cmp","LDRI","LDRI"},
};
static struct directive_definition dir_table[4] = {
        {dir_external,".extern","L"},
        {dir_entry,".entry","L"},
        {dir_data,".data","^"},
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
    }type;
    union {
        char *symbol;
        long immed;
        long reg;
        struct{
            char * symbol;
            int index_num_or_symbol;
            int index;
            char * index_symbol;
        }array_index;
    }result;
};

/**
 * @brief 
 * 
 * @param number_candidate 
 * @param result 
 * @param endptr_space 
 * @param max 
 * @param min 
 * @return int -1 overflow , -2 no number
 */
static int my_strtol(char * number_candidate, long * result,char **endptr_space,int max, int min) {
    errno = 0;
    *result = strtol(number_candidate,endptr_space,10);
    if(errno == ERANGE || *result > max || *result < min)
        return -1;
    if(*endptr_space == number_candidate)
        return -2;
    after_sppace(*endptr_space);
    return 0;
}

static int is_valid_symbol(char *symbol_candidate,char ** symbol_ret);

static struct parse_args_result parse_args(char * args_string,const char * args_allow) {
    after_sppace(args_string);
    if(*args_string == '#') {
        args_string++;

    }
    else if(*args_string == 'r') {

    }
}

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
            ast->ast_options.ast_op.operands[src_or_dest].operand.array_index.symbol = par->result.array_index.symbol;
            if(par->result.array_index.index_num_or_symbol == array_index_label) {
                ast->ast_options.ast_op.operands[src_or_dest].operand.array_index.index_symbol = par->result.array_index.index_symbol;
            }else {
                ast->ast_options.ast_op.operands[src_or_dest].operand.array_index.index = par->result.array_index.index;
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
    separator = strchr(args_string,',');
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
    char *t1,*t2;
    after_sppace(args_string);
    if(*args_string == '\0') {
        sprintf(ast->syntax_error,"no arguments provided for directive:'%s'.",asm_dir_def->name);
        return;
    }
    switch(asm_dir_def->adt) {
        case dir_entry: case dir_external:
            t1 = strpbrk(args_string,SPACES);
            if(t1) {
                after_sppace(t1);
                t2 = strpbrk(t1,SPACES);
                if(t2) {
                    *t2 = '\0';
                    t2++;
                    after_sppace(t2);
                    if(*t2 != '\0') {
                        sprintf(ast->syntax_error,"extra text after symbol for %s directive.",asm_dir_def->name);
                    }
                }
                ast->ast_options.ast_dir.dir_option.symbol = t1;
            }else {
                sprintf(ast->syntax_error,"missing symbol for %s directive.",asm_dir_def->name);
            }
            break;
        case dir_data:
        
            break;
        case dir_string:
            t1 = strchr(args_string,'"');
            if(t1) {
                t2 = strrchr(args_string,'"');
                if(t1 == t2) {
                    sprintf(ast->syntax_error,"missing closing '\"' for %s directive.",asm_dir_def->name);
                }else {
                    *t2 = '\0';
                    t2++;
                    after_sppace(t2);
                    if(*t2 !='\0') {
                        sprintf(ast->syntax_error," text after closing '\"' for %s directive.",asm_dir_def->name);
                    }else {
                        ast->ast_options.ast_dir.dir_option.string = t1+1;
                    }
                }
            }else {
                sprintf(ast->syntax_error,"missing opening '\"' for %s directive.",asm_dir_def->name);
            }
            break;
    }
}
static enum asm_op_type lexer_get_op_type(char *type) {
    int i;
    for(i=0;i<16;i++) {
        if(strcmp(type,asm_table[i].name) == 0) {
            return asm_table[i].aot;
        }
    }
    return ast_op_undefined;
}
static enum asm_dir_type lexer_get_dir_type(char *type) {
    int i;
    for(i=0;i<4;i++) {
        if(strcmp(type,dir_table[i].name) == 0) {
            return dir_table[i].adt;
        }
    }
    return ast_dir_undefined;
}
/**
 * @brief 
 * 
 * @param aot 
 * @param adt 
 * @param args 
 * @return int 0 - aot, 1 - adt, 2 - define ( const number) , 3 - undefined
 */
static enum ast_type lexer_get_type(char * line,enum asm_op_type * aot, enum asm_dir_type *adt, char ** args) {
    char * t1;
    t1 = strpbrk(line,SPACES);
    if(t1) {
        *t1 ='\0';
        t1++;
        after_sppace(t1);
        *args = t1;
    }
    *args = NULL;
    *aot = lexer_get_op_type(line);
    if(*aot != ast_op_undefined)
        return ast_operation;
    *adt = lexer_get_dir_type(line);
    if(*adt != ast_dir_undefined) {
        return ast_directive;
    }
    if(strcmp(line,".define") == 0) {
        return ast_const_def;
    }
}
struct ast lexer_get_ast(char * line) {
    struct ast new_ast = {0};
    char * args;
    char * t1;
    /* algo goes here...*/
    line[strcspn(line, "\r\n")] = 0;
    after_sppace(line);
    switch(lexer_get_type(line,&new_ast.ast_options.ast_op.aot,&new_ast.ast_options.ast_dir.adt,&args)) {
        case ast_operation:
            parse_asm_args(args,&asm_table[new_ast.ast_options.ast_op.aot],&new_ast);
        break;
        case ast_directive:
            parse_dir_args(args,&dir_table[new_ast.ast_options.ast_dir.adt],&new_ast);
        break;
        case ast_const_def:
            if(*args == '\0') {
                sprintf(new_ast.syntax_error,"missing argument for .define.");
            }else {
                switch(my_strtol(args,&new_ast.ast_options.define_num,&t1,C_MAX,C_MIN)) {
                    case 0:
                        if(*t1 != '\0'){
                            sprintf(new_ast.syntax_error,"extra argument'%s' for .define.",t1);
                        }
                    break;
                    case -1:
                        sprintf(new_ast.syntax_error,"number:'%s'  for define overflows",args);
                    break;
                    case -2:
                        sprintf(new_ast.syntax_error,"arguments:'%s' for .define are invalid.",args);
                    break;
                }
            }
        break;
        case ast_undefined:
            sprintf(new_ast.syntax_error,"undefined keyword:'%s'.",line);
        break;
    }
    return new_ast;
}