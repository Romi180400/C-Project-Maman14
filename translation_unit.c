#include "translation_unit.h"
#include <string.h>
struct symbol * symbol_table_search(struct translation_unit * tu,const char * candidate) {
    int i;
    for(i=0;i<tu->symbol_table_size;i++) {
        if(strcmp(tu->symbol_table[i].name,candidate) == 0) {
            return &tu->symbol_table[i];
        }
    }
    return NULL;
}
void symbol_table_insert(struct translation_unit * tu, const char *candidate, 
                        enum symbol_type symbol_type,
                        int address,int line_of_def,int c_number,int data_or_str_size) {
    strcpy(tu->symbol_table[tu->symbol_table_size].name,candidate);
    tu->symbol_table[tu->symbol_table_size].symbol_type = symbol_type;
    tu->symbol_table[tu->symbol_table_size].address = address;
    tu->symbol_table[tu->symbol_table_size].constant_number = c_number;
    tu->symbol_table[tu->symbol_table_size].line_of_def =line_of_def;
    tu->symbol_table[tu->symbol_table_size].data_or_str_size = data_or_str_size;
    tu->symbol_table_size++;
}
struct external * symbol_table_search_external(struct translation_unit * tu,const char * candidate) {
int i;
for(i=0; i < tu->external_arr_size;i++) {
    if(strcmp(candidate,tu->external_arr[i].name) == 0) {
        return &tu->external_arr[i];
    }
}
return NULL;

}