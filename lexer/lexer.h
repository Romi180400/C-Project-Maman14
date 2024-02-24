#ifndef __LEX_H
#define __LEX_H
enum asm_op_type{
    ast_mov,
    ast_cmp,
    ast_add
};

enum asm_dir_type{
    dir_entry,
    dir_external,
    dir_data,
    dir_string
};

struct ast {
    char syntax_error[200];
    enum  {
        ast_operation,
        ast_directive,
        ast_const_def
    }ast_type;
    union {
        struct {
            enum asm_op_type aot;
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
                        struct { /* symbol AND index*/
                            char * symbol;
                            enum {
                                array_index_label,
                                array_index_number
                            }index_num_or_symbol;
                            int index;
                            char *label;
                        }index_pair;
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
                    int data[90];
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
