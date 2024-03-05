#ifndef __LEX_H
#define __LEX_H
enum asm_op_type{
    ast_mov,
    ast_cmp,
    ast_add,

    ast_op_undefined
};

enum asm_dir_type{
    dir_entry,
    dir_external,
    dir_data,
    dir_string,
    ast_dir_undefined
};
    enum ast_type {
        ast_operation,
        ast_directive,
        ast_const_def,
        ast_undefined
    };
struct ast {
    char syntax_error[200];
    char symbol[32];
    enum ast_type ast_type;
    union {
        struct {
            enum asm_op_type aot;
            struct {
                enum {
                    operand_none = 0,
                    operand_immd = 0,
                    operand_immd_symbol =0,
                    operand_symbol,
                    operand_array_index,
                    operand_register
                }operand_option;
                union {
                    int immd;
                    int reg;
                    char * symbol;
                    struct {
                        char * symbol;
                        enum {
                            array_index_label,
                            array_index_number
                        }index_num_or_symbol;
                        int index;
                        char *index_symbol;
                    }array_index;
                }operand;
            }operands[2];
        }ast_op;
        struct {
            enum asm_dir_type adt;
            union {
                char * symbol;
                char * string;
                struct {
                    enum {
                        data_define_symbol,
                        data_const_num
                    }data_type[82];
                    union {
                        char * define_symbol;
                        int number;
                    }data_options[82];
                    int data_count;
                }data;
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
struct ast lexer_get_ast( char * line);

#endif
