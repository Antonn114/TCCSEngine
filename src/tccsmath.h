#ifndef __TCCSMATH_H__
#define __TCCSMATH_H__

#include <stdlib.h>
#include <stdint.h>
#include <assert.h>
#include <math.h>

void swapi32(int* lhs, int* rhs);
float cross_product(float ax, float ay, float bx, float by);
float normal(float x1, float y1, float x2, float y2, float x3, float y3);

/*
 * Linear Algebraic Structures
 **/

typedef struct {
    float** m;
    size_t r, c;
} TCCSMatrixF32;

TCCSMatrixF32* TCCS_matrix_f32_create(size_t r, size_t c);
TCCSMatrixF32* TCCS_matrix_f32_multiply(TCCSMatrixF32* lhs, TCCSMatrixF32* rhs);
void TCCS_matrix_f32_destroy(TCCSMatrixF32* mat);


#endif /* __TCCSMATH_H__ */
