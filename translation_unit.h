#ifndef TU_H_
#define TU_H_
    enum symbol_type {
        symbol_code,
        symbol_data,
        symbol_entry, /* temporary state*/
        symbol_entry_data,
        symbol_entry_code,
        symbol_external,
        symbol_const_number /* define*/
    };
struct symbol  {
    char name[32];
    enum symbol_type symbol_type;
    int address;
    int line_of_def;
    int constant_number;
    int data_or_str_size;
};
struct external {
    const char * name;
    int addresses[4096];
    int addr_count;
};

/**
 * @brief 
 * 
 */
struct translation_unit {
    int code_section[4096];
    int code_section_size;
    int data_section[4096];
    int data_section_size;

    struct symbol symbol_table[4096];
    int symbol_table_size;

    const struct symbol * entries[4096];
    int entries_count; 
    struct external external_arr[4096];
    int external_arr_size;
};

struct symbol * symbol_table_search(struct translation_unit * tu,const char * candidate);
void symbol_table_insert(struct translation_unit * tu, const char *candidate, 
                        enum symbol_type symbol_type,
                        int address,int line_of_def,int c_number,int data_or_str_size);

#endif