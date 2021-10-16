/* Made by Gabriel Lüders
   GRR20190172 */

#include <dirent.h>
#include <string.h>
#include <stdlib.h>
#include "dir.h"
#include "utils.h"

// Checks if a certain string has the extension .ppm
// returns true if positive
int MyFilter(const struct dirent* entry){

  // finds a pointer to the last '.' in the string name
  char *ptr = strrchr (entry->d_name, '.');
  if(!ptr) return 0;

  // compares the string from the '.' found
  // returns true if == '.ppm'
  return !strcmp(ptr, ".ppm");
}

// responsible for getting all the files at
// path "dir" according to "MyFilter" and utilizing
// scandir
// returns a pointer to an array of dirent
struct dirent** GetDirEntries(char *dir, long* n){
  struct dirent **list = NULL;
  *n = scandir(dir, &list, MyFilter, NULL);

  if(!list)
    FireErrorException(__func__);

  return list;
}

// responsible for freeing all memory allocated for dirents
// and for nullifying all dirent pointers
void FreeDirEntries(struct dirent** list){
  // if the list has already been freed, retuns out
  if(!(*list)) return;

  free(*list);
  *list = NULL;
}