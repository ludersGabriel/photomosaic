/* Made by Gabriel Lüders
   GRR20190172 */

#ifndef MATRIX_H
#define MATRIX_H

// allocates memory for a bidimensional array 
// based on varSize
void** AllocateMatrix(long varSize, long lin, long col);

// frees and nullifies all array's pointers allocated
// by this specific memory allocation method
void FreeMatrix(void*** mat);

#endif