#include "translation_unit.h"
#include "utils.h"
#include <stdio.h>

#define OUT_PRNT_STR "%c%c%c%c%c%c%c"

#define OUT_STR "*#%!"
#define OUT_PRNT_BITS(bits) OUT_STR[((bits >> 12 ) & 0x3)], OUT_STR[((bits >> 10 ) & 0x3)],OUT_STR[((bits >> 8 ) & 0x3)],OUT_STR[((bits >> 6 ) & 0x3)],OUT_STR[((bits >> 4 ) & 0x3)],OUT_STR[((bits >> 2 ) & 0x3)],OUT_STR[((bits >> 0 ) & 0x3)]
static void back_print_ext( const struct external *externals, const int externals_size,const char *b_name) {
    char *ext_f_name;
    FILE *ext_file;
    int i;
    int j;
    ext_f_name = m_strcat(b_name,".ext");
    ext_file = fopen(ext_f_name,"w");
    if(ext_file) {
     for(i=0;i<externals_size;i++) {
        for(j=0;j<externals[i].addr_count;j++) {
            fprintf(ext_file,"%s\t%d\n",externals[i].name,externals[i].addresses[j]);
        }
    }
    fclose(ext_file);      
    }
    free(ext_f_name);
}
static void back_print_ent(const struct symbol * const entries[], const int entries_size,const char *b_name) {
    char *ent_f_name;
    FILE *ent_file;
    int i;
    ent_f_name = m_strcat(b_name,".ent");
    ent_file = fopen(ent_f_name,"w");
    if(ent_file) {
        for(i=0;i<entries_size;i++) {
            fprintf(ent_file,"%s\t%d\n",entries[i]->name,entries[i]->address);
        }
        fclose(ent_file);
    }
    free(ent_f_name);
}
static void back_print_ob(const int *code_section,
                          const int code_section_size,
                          const int *data_section,const int data_section_size,const char *b_name) {
    int i;
    FILE * ob_file;
    char * ob_file_name;
    int addr = 100;
    ob_file_name = m_strcat(b_name,".ob");
    ob_file = fopen(ob_file_name,"w");
    if(ob_file) {
        fprintf(ob_file,"  %d %d\n",code_section_size,data_section_size);
        for(i=0;i<code_section_size;i++,addr++) {
            fprintf(ob_file,"%04d ",addr);
            fprintf(ob_file,OUT_PRNT_STR "\n",OUT_PRNT_BITS(code_section[i]));
        }

        for(i=0;i<data_section_size;i++,addr++) {
            fprintf(ob_file,"%04d ",addr);
            fprintf(ob_file,OUT_PRNT_STR "\n",OUT_PRNT_BITS(data_section[i]));

        }
    fclose(ob_file);
    }
    free(ob_file_name);
}
void back_main(const struct translation_unit *tu,const char *b_name) {
    if(tu->entries_count >0) {
        back_print_ent(tu->entries,tu->entries_count,b_name);
    }
    if(tu->external_arr_size >0) {
        back_print_ext(tu->external_arr,tu->external_arr_size,b_name);
    }
    if(tu->code_section_size >0 || tu->data_section_size >0) {
        back_print_ob(tu->code_section,tu->code_section_size,tu->data_section,tu->data_section_size,b_name);
    }
}
