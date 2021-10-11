/* Feito Por Gabriel Lüders
   GRR20190172 */

#ifndef DIR_H
#define DIR_H

#include <dirent.h>

// Checks if a certain string has the extension .ppm
// returns true if positive
int MyFilter(const struct dirent* entry);

// responsible for getting all the files at
// path "dir" according to "MyFilter" and utilizing
// scandir
// returns a pointer to an array of dirent
struct dirent** GetDirEntries(char *dir, long* n);

// responsible for freeing all memory allocated for dirents
// and for nullifying all dirent pointers
void FreeDirEntries(struct dirent** list);

#endif