#include "preproc.h"
#include "utils.h"
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#define after_sppace(s) while(isspace(*s))s++
#define SPACES " \t\f\r\v"
struct macro {
    char name[32];
    char lines[10][82];
    int line_c;
};

struct macro_table {
    struct macro macros[1000];
    int macro_count;
};

enum preproc_line_type {
    macro_def,
    macro_def_err_missing_name,
    macro_redef_err,
    macro_end_def,
    macro_call,
    preproc_other,
    preproc_null_line
};
static struct macro * search_macro(const struct macro_table * m_table,const char *m_name) {
    int i;
    for(i=0;i<m_table->macro_count;i++) {
        if(strcmp(m_table->macros[i].name,m_name) == 0) {
            return (struct macro*)&m_table->macros[i];
        }
    }
    return NULL;
}
static enum preproc_line_type preproc_get_line_type(char *line,struct macro_table * mcr_table,struct macro **macro_ptr) {
    char line_copy[82] = {0};
    char *t1,*t2;
    after_sppace(line);
    strcpy(line_copy,line);
    line = line_copy;
    line[strcspn(line, "\r\n")] = 0;
    if(*line == '\0')
        return preproc_null_line;
    t1 = strstr(line,"endmcr");
    if(t1) {
        return macro_end_def;
    }
    t1 = strstr(line,"mcr");
    if(t1) {
        t1+=3;
        after_sppace(t1);
        if(*t1 =='\0') {
            return macro_def_err_missing_name;
        }
        t2 = strpbrk(line,SPACES);
        if(t2) {
            *t2 =0;
        }
        *macro_ptr = search_macro(mcr_table,t1);
        if(*macro_ptr) {
            return macro_redef_err;
        }
        else {
            strcpy(mcr_table->macros[mcr_table->macro_count].name,t1);
            *macro_ptr = &mcr_table->macros[mcr_table->macro_count];
            mcr_table->macro_count++;
        }
    }
    t2 = t1 = strpbrk(line,SPACES);
    if(t1) {
        after_sppace(t1);
        if(*t1 == '\0') {
            *t2 = '\0';
        }else {
            return preproc_other;
        }
    }
    *macro_ptr = search_macro(mcr_table,line);
    if(*macro_ptr) {
        return macro_call;
    }
    return preproc_other;
}


char * pre_processor(const char *b_name) {
    char * am_name;
    char * as_name;
    int i;
    int line_counter = 1;
    char *t;
    char *t1,*t2;
    char buffer[82] = {0};
    FILE * as_file, * am_file;
    struct macro_table mcr_tbl = {0};
    struct macro * macro_ptr;
    as_name = m_strcat(b_name,".as");
    am_name = m_strcat(b_name,".am");
    as_file = fopen(as_name,"r");
    am_file = fopen(am_name,"w");
    if(! am_file || ! as_file) {
        asm_prnt_err(as_name,line_counter   ,"could not open file: %s or file: %s .",as_name,am_name);
        free(am_name);
        free(as_name);
        return NULL;
    }
    while(fgets(buffer,sizeof(buffer),as_file)) {
        t = strchr(buffer,';');
        if(t) {
            t1 = strchr(buffer,'"');
            if(t1) {
                t2 = strrchr(buffer,'"');
                if(t > t2 || t < t1) {
                    *t = '\0';
                }
            }else {
                *t = '\0';
            }
        }
        switch(preproc_get_line_type(buffer,&mcr_tbl,&macro_ptr)) {
            case macro_def:
            break;
            case macro_def_err_missing_name:
                asm_prnt_err(as_name,line_counter,"missing macro name after mcr.");
            break;
            case macro_redef_err:
                asm_prnt_err(as_name,line_counter,"redefinition of macro:'%s'.",macro_ptr->name);
                macro_ptr = NULL;
            break;
            case macro_end_def:
                macro_ptr = NULL;
            break;
            case macro_call:
                for(i=0;i>macro_ptr->line_c;i++) {
                    fputs(macro_ptr->lines[i],am_file);
                }
                macro_ptr = NULL;
            break;
            case preproc_other:
                if(macro_ptr) {
                    strcpy(macro_ptr->lines[macro_ptr->line_c],buffer);
                    macro_ptr->line_c++;
                }else {
                    fputs(buffer,am_file);
                }
            break;
            case preproc_null_line:
            break;
        }
        line_counter++;
    }
    fclose(am_file);
    fclose(as_file);
    free(as_name);
    return am_name;
}
