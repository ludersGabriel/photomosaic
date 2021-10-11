/* Feito Por Gabriel Lüders
   GRR20190172 */

#include <stdlib.h>
#include <stdio.h>
#include "matrix.h"

// allocates memory for a bidimensional array 
// based on varSize
void** AllocateMatrix(long varSize, long lin, long col){
  void **mat;

  // allocate the lign vector
  mat = malloc(lin * sizeof(void*));
  if(!mat){
    perror("Erro ao alocar ponteiros para as linhas\n");
    exit(1);
  }

  // allocate space for all matrix's positions
  mat[0] = malloc(lin * col * varSize);
  if(!mat[0]){
    perror("Erro ao alocar o ponteiro do conteúdo");
    exit(1);
  }

  // corrects the pointers from lign > 0
  for(long i = 1; i < lin; i++)
    mat[i] = mat[0] + i * col * varSize;

  return mat;
}

// frees and nullifies all array's pointers allocated
// by this specific memory allocation method
void FreeMatrix(void*** mat){
  if(!(*mat)) return;
  free((*mat)[0]);
  (*mat)[0] = NULL;
  free(*mat);
  *mat = NULL;
}