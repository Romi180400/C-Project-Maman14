#ifndef __LEX_H
#define __LEX_H
#define MAX_ERROR 200
#define MAX_NUMS_OP 2
#define MAX_DATA 90


struct ast {
    char syntax_error[MAX_ERROR];
    enum  {
        ast_operation,
        ast_directive,
        ast_const_def
    }ast_type;
    union {
        struct {
            enum {
                ast_mov,
                ast_cmp,
                ast_add,
                ast_sub,
                ast_not,
                ast_clr,
                ast_lea,
                ast_inc,
                ast_dec,
                ast_jmp,
                ast_bne,
                ast_red,
                ast_prn,
                ast_jsr,
                ast_rst,
                ast_hlt
            }op_type;
            struct {
                enum {
                    operand_immd,
                    operand_symbol,
                    operand_array_index,
                    operand_register
                }operand_option;
                union {
                    int immd;
                    int reg;
                    char * symbol;
                    struct {
                        enum {
                            array_index_symbol,
                            array_index_number
                        }index_num_or_symbol;
                        union {
                            char * symbol;
                            int index;
                        }index_num_or_symbol_union;
                    }array_index;
                }operand;
            }operands[MAX_NUMS_OP];
        }ast_op;
        struct {
            enum {
                dir_entry,
                dir_external,
                dir_data,
                dir_string
            }dir_type;
            union {
                char * symbol;
                char * string;
                struct {
                    int data[MAX_DATA];
                    int array_size;
                }data_array;
            }dir_option;
        }ast_dir;

        struct {
            char *define_symbol_name;
            int number;
        }define_num;

    }ast_options;
};

/**
 * @brief creates AST for a line
 *
 * @param line
 * @return struct ast
 */
struct ast lexer_get_ast(const char * line);

#endif
