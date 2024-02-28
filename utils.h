#ifndef UTILS_H
#define UTILS_H
#include <string.h>
#include <stdlib.h>

#define m_strcpy(s) strcpy(malloc(strlen(s) + 1),s)

#define m_strcat(str,catstr) strcat(strcpy(malloc(strlen(str) + strlen(catstr) + 1),str),catstr)

#endif