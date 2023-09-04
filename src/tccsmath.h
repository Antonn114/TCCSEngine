#ifndef __TCCSMATH_H__
#define __TCCSMATH_H__

#include <stdlib.h>
#include <stdint.h>
#include <math.h>

void swapi32(int* lhs, int* rhs);

/*
 * Linear Algebraic Structures
 **/

typedef struct {
    float** m;
    size_t r, c;
} TCCSMatrixF32;

TCCSMatrixF32* TCCS_matrix_f32_create(size_t r, size_t c);
void TCCS_matrix_f32_destroy(TCCSMatrixF32* mat);


#endif /* __TCCSMATH_H__ */
