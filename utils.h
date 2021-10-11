/* Feito Por Gabriel Lüders
   GRR20190172 */

#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>

// opens the file given by path in the mode = mode
// returns a file pointer for the given path
FILE* OpenFile(char* path, char* mode);

// allocates space and concatanates two strings
// in the order string1+string2
// returns the resulting concatanation
char* AppendString(char* str1, char* str2);

// adds a \0 in the last 'c' found in the string
void FilterString(const char* str, char c);

// checks the command line arguments and set default 
// values when needed
void ParseArgs(int argc, char** argv, const char *args, FILE** valueI, FILE** valueO, char** valueP);

// throws a generic error
void FireErrorException(const char* funcName);

// thows a memory error 
void FireMemErrorException(const char* funcName);

#endif