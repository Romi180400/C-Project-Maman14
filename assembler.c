#include <stdio.h>
#include "lexer/lexer.h"
#include "preproc.h"
#include "translation_unit.h"
#include <string.h>
#include "utils.h"
#define PROG_BASE 100

void back_main(const struct translation_unit *tu,const char *b_name);
/**
 * @brief builds the program
 *
 * @return int
 */
static int assembler_second_pass(struct translation_unit * tu,FILE * am_file, const char *am_file_name) {
    char line_buffer[100] = {0};
    struct ast AST;
    int str_len;
    struct symbol * symbol_s, * symbol_s2;
    int ok_flag =1;
    int line_counter = 1;
    int machine_code;
    int i;
    rewind(am_file);
    while(fgets(line_buffer,sizeof(line_buffer),am_file)) {
        AST = lexer_get_ast(line_buffer);
        if(AST.ast_type == ast_directive && AST.ast_options.ast_dir.adt > dir_external) {
            if(AST.ast_options.ast_dir.adt == dir_string) {
                str_len = strlen(AST.ast_options.ast_dir.dir_option.string);
                memcpy(&tu->data_section[tu->data_section_size],AST.ast_options.ast_dir.dir_option.string,str_len);
                tu->data_section_size += str_len +1;
            }else {
                for(i=0;i<AST.ast_options.ast_dir.dir_option.data.data_count;i++) {
                    if(AST.ast_options.ast_dir.dir_option.data.data_type[i] == data_define_symbol) {
                        symbol_s = symbol_table_search(tu,AST.ast_options.ast_dir.dir_option.data.data_options[i].define_symbol);
                        if(symbol_s) {
                            machine_code = symbol_s->constant_number;
                        }else {
                            asm_prnt_err(am_file_name,line_counter,"undefined symbol:'%s'.",symbol_s->name);
                            ok_flag = 0;
                        }
                    }else {
                        machine_code =AST.ast_options.ast_dir.dir_option.data.data_options[i].number;
                    }
                    tu->data_section[tu->data_section_size] = machine_code;
                    tu->data_section_size++;
                }
            }
        }else if(AST.ast_type == ast_operation) {
            machine_code = AST.ast_options.ast_op.aot << 6;
            machine_code |= AST.ast_options.ast_op.operands[0].operand_option << 4;
            machine_code |= AST.ast_options.ast_op.operands[1].operand_option << 2;
            tu->code_section[tu->code_section_size] = machine_code;
            tu->code_section_size++;
            if(AST.ast_options.ast_op.operands[0].operand_option == operand_register &&
                 AST.ast_options.ast_op.operands[1].operand_option == operand_register ) {
                            machine_code = AST.ast_options.ast_op.operands[1].operand.reg << 3;
                            machine_code |= AST.ast_options.ast_op.operands[0].operand.reg << 5;
                            tu->code_section[tu->code_section_size] = machine_code;
                            tu->code_section_size++;
            }else {
                for(i=0;i<2;i++) {
                    if(AST.ast_options.ast_op.operands[i].operand_option == operand_none) {
                        continue;
                    }
                    if(AST.ast_options.ast_op.operands[i].operand_option >= operand_immd &&
                        AST.ast_options.ast_op.operands[i].operand_option!= operand_array_index ) {
                            if(AST.ast_options.ast_op.operands[i].operand_option == operand_register) {
                                machine_code = AST.ast_options.ast_op.operands[i].operand.reg << (5 - (i * 3));
                            }else if(AST.ast_options.ast_op.operands[i].operand_option == operand_immd) {
                                machine_code = AST.ast_options.ast_op.operands[i].operand.immd << 2;
                            }else {
                                symbol_s = symbol_table_search(tu,AST.ast_options.ast_op.operands[i].operand.symbol);
                                if(symbol_s) {
                                    if(AST.ast_options.ast_op.operands[i].operand_option == operand_immd_symbol) {
                                        if(symbol_s->symbol_type != symbol_const_number) {
                                            ok_flag = 0;
                                            /* error , symbol:... expected to be constant number symbol*/
                                        }else {
                                            machine_code = symbol_s->constant_number << 2;
                                        }
                                    }else {
                                        if(symbol_s->symbol_type == symbol_const_number) {
                                            ok_flag = 0;
                                            /* error  symbol.... expected not to be a symbol immed*/
                                        }else {
                                            machine_code = symbol_s->address << 2;
                                            if(symbol_s->symbol_type == symbol_external) {
                                                machine_code |= 1;
                                            }else {
                                                machine_code |= 2;
                                            }
                                        }
                                    }
                                }else {
                                    ok_flag = 0;
                                    /* error , symbol : wast not found in symbol table*/
                                }
                            }
                            tu->code_section[tu->code_section_size] = machine_code;
                            tu->code_section_size++;
                    }
                    else { /* operand_array_index */
                        symbol_s = symbol_table_search(tu,AST.ast_options.ast_op.operands[i].operand.array_index.symbol);
                        if(symbol_s) {
                            if(symbol_s->symbol_type != symbol_entry_data && symbol_s->symbol_type != symbol_data) {

                                ok_flag =0;
                                /* symbol X is not a data symbol and therefore cannot use array index*/
                            }
                            tu->code_section[tu->code_section_size++] = (symbol_s->address << 2) | 2; 
                            if(AST.ast_options.ast_op.operands[i].operand.array_index.index_num_or_symbol == array_index_number) {
                                if(AST.ast_options.ast_op.operands[i].operand.array_index.index <0 ||
                                    (AST.ast_options.ast_op.operands[i].operand.array_index.index >= symbol_s->data_or_str_size )) {
                                            /* symbol X with index Y is outside of bounds of this array/string*/
                                    ok_flag =0;
                                }else {
                                    tu->code_section[tu->code_section_size++] = AST.ast_options.ast_op.operands[i].operand.array_index.index << 2;
                                }
                            }else {
                                symbol_s2 = symbol_table_search(tu,AST.ast_options.ast_op.operands[i].operand.array_index.index_symbol);
                                if(symbol_s2 && symbol_s2->symbol_type == symbol_const_number) {
                                    if(symbol_s2->constant_number <0 ||
                                        (symbol_s2->constant_number >= symbol_s->data_or_str_size )) {
                                            /* symbol X with index Y is outside of bounds of this array/string*/
                                        ok_flag =0;
                                    }
                                    else {
                                        tu->code_section[tu->code_section_size++] = symbol_s2->constant_number << 2;
                                    }
                                }else {
                                    ok_flag = 0;
                                     /* error , symbol : wast not found in symbol table or the symbol was not defined as a const number*/                                   
                                }
                            }
                        }else {
                            ok_flag = 0;
                            /* error , symbol : wast not found in symbol table*/
                        }
                    }

                }
            }
        }
        line_counter++;
    }
    return ok_flag;
}
/**
 * @brief builds the symbol table
 *
 * @return int
 */
static int assembler_first_pass(struct translation_unit * tu,FILE * am_file, const char *am_file_name) {
    char line_buffer[100] = {0};
    struct ast AST;
    struct symbol * symbol_s;
    int ok_flag =1;
    int line_counter = 1;
    int IC = PROG_BASE, DC = 0;
    int i;
    while(fgets(line_buffer,sizeof(line_buffer),am_file)) {
        AST = lexer_get_ast(line_buffer);
        if(AST.syntax_error[0] != '\0') {
            ok_flag = 0;
            /* remember to print somehow the syntax error*/
            asm_prnt_err(am_file_name,line_counter,"syntax error: %s.",AST.syntax_error);
        }else {
            if(AST.ast_type == ast_directive && AST.ast_options.ast_dir.adt <= dir_external ) {
                symbol_s = symbol_table_search(tu,AST.ast_options.ast_dir.dir_option.symbol);
                if(symbol_s) {
                    if(AST.ast_options.ast_dir.adt == dir_external ) {
                        ok_flag = 0;
                        /* remember to print error , this symbol is being declared as external but was already defined*/
                        asm_prnt_err(am_file_name,line_counter,"symbol:'%s' was declared as external in line:'%s' symbol and now is being defined.",symbol_s->line_of_def);
                    }
                    else {
                        if(symbol_s->symbol_type == symbol_code) {
                            symbol_s->symbol_type = symbol_entry_code;
                        }
                        else if(symbol_s->symbol_type == symbol_data) {
                            symbol_s->symbol_type = symbol_entry_data;
                        }else {
                            ok_flag = 0;
                            /* remember to print error , this symbol is being declared as entry but was already defined*/         
                            asm_prnt_err(am_file_name,line_counter,"symbol:'%s' was already defined in line: %d",symbol_s->line_of_def);              
                        }
                    }

                }else {
                    if(AST.ast_options.ast_dir.adt == dir_external ) {
                        symbol_table_insert(tu,AST.ast_options.ast_dir.dir_option.symbol,symbol_external,0,line_counter,0,0);
                    }else {
                        symbol_table_insert(tu,AST.ast_options.ast_dir.dir_option.symbol,symbol_entry,0,line_counter,0,0);
                    }
                }
            }else if(AST.symbol != NULL) {
                symbol_s = symbol_table_search(tu,AST.symbol);
                if(symbol_s) {
                    if(symbol_s->symbol_type == symbol_entry) {
                        if(AST.ast_type == ast_operation) {
                            symbol_s->symbol_type = symbol_entry_code;
                            symbol_s->address = IC;
                            symbol_s->line_of_def = line_counter;
                        }
                        else if(AST.ast_type == ast_directive) {
                            symbol_s->symbol_type = symbol_entry_data;
                            symbol_s->address = DC;
                            symbol_s->line_of_def = line_counter;
                        }
                        else {
                            
                            ok_flag =0;
                            /* remember to print logical error , redefintion of symbol*/
                            asm_prnt_err(am_file_name,line_counter,"symbol:'%s' was already defined in line: %d",symbol_s->name,symbol_s->line_of_def);   
                        }
                    }
                }else {
                    if(AST.ast_type == ast_operation) {
                        symbol_table_insert(tu,AST.symbol,symbol_code,IC,line_counter,0,0);
                    }
                    else {
                        if(AST.ast_options.ast_dir.adt == dir_string) {
                            symbol_table_insert(tu,AST.symbol,symbol_data,DC,line_counter,0,
                            strlen(AST.ast_options.ast_dir.dir_option.string) + 1);
                        }else {
                            symbol_table_insert(tu,AST.symbol,symbol_data,DC,line_counter,0,
                            AST.ast_options.ast_dir.dir_option.data.data_count);
                        }
                        
                    }
                    
                }
            }
            if(AST.ast_type == ast_directive && AST.ast_options.ast_dir.adt > dir_external ) {
                if(AST.ast_options.ast_dir.adt == dir_data) {
                    DC += AST.ast_options.ast_dir.dir_option.data.data_count;
                }else {
                for(i=0;i<tu->symbol_table_size;i++) {
                    if(tu->symbol_table[i].symbol_type == symbol_data || tu->symbol_table[i].symbol_type == symbol_entry_data) {
                        tu->symbol_table[i].address += IC;
                    }
            } DC += strlen(AST.ast_options.ast_dir.dir_option.string) + 1;
                }
            }else if(AST.ast_type == ast_operation) {
                IC++;
                if(AST.ast_options.ast_op.operands[0].operand_option == operand_register &&
                 AST.ast_options.ast_op.operands[1].operand_option == operand_register ) {
                    IC++;
                }
                for(i=0;i<2;i++) {
                    if(AST.ast_options.ast_op.operands[i].operand_option >= operand_immd && AST.ast_options.ast_op.operands[i].operand_option!= operand_array_index ) {
                        IC++;
                    }
                    else {
                        IC+=2;
                    }
                }
            }
        }
        line_counter++;
    }
    for(i=0;i<tu->symbol_table_size;i++) {
        if(tu->symbol_table[i].symbol_type == symbol_entry) {
            /* remember to print error , symbol was declared entry but was never defined*/
            asm_prnt_err(am_file_name,0,"symbol:'%s' was declared entry but was never defined in this file.",tu->symbol_table[i].name);
            ok_flag = 0;
        }
        if(tu->symbol_table[i].symbol_type == symbol_data || tu->symbol_table[i].symbol_type == symbol_entry_data) {
            tu->symbol_table[i].address += IC;
        }
    }
    return ok_flag;
}






int main(int argc, char ** argv) {
    FILE * am_FILE;
    int i;
    static struct translation_unit tu_for_each = {0};
    char * am_file_name;
    argc--;argv++;
    for(i=0;i<argc;i++) {
        if( (am_file_name = pre_processor(argv[i])) ) {
            am_FILE = fopen(am_file_name,"r");
            if(am_FILE) {
                memset(&tu_for_each,0,sizeof(tu_for_each));
                if(assembler_first_pass(&tu_for_each,am_FILE,am_file_name)) {
                    if(assembler_second_pass(&tu_for_each,am_FILE,am_file_name)) {
                        back_main(&tu_for_each,argv[i]);
                    }
                }
            }
        }
    }
    return 0;
}
