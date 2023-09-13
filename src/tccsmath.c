#include "tccsmath.h"

void swapi32(int *x, int *y){
    *x ^= *y;
    *y ^= *x;
    *x ^= *y;
}

float cross_product(float ax, float ay, float bx, float by){
    return ax*by - ay*bx;
}

float normal(float x1, float y1, float x2, float y2, float x3, float y3){
    return cross_product(x2 - x1, y2 - y1, x3 - x2, y3 - y2);
}

TCCSMatrixF32* TCCS_matrix_f32_create(size_t r, size_t c){
    TCCSMatrixF32 *mat = (TCCSMatrixF32*) malloc(sizeof(TCCSMatrixF32));
    if (mat){
        mat->r = r;
        mat->c = c;
        mat->m = (float**) malloc(sizeof(float*) * r);
        if (mat->m){
            size_t i;
            for (i = 0; i < r; i++){
                mat->m[i] = (float*) malloc(sizeof(float)*c);
            }
        }else{
            free(mat);
            return NULL;
        }
    }else{
        return NULL;
    }
    return mat;
}

TCCSMatrixF32* TCCS_matrix_f32_multiply(TCCSMatrixF32* lhs, TCCSMatrixF32 *rhs){
    assert(lhs->c == rhs->r);
    TCCSMatrixF32* res = TCCS_matrix_f32_create(lhs->r, rhs->c);
    size_t i, j, k;
    for (i = 0; i < lhs->r; i++){
        for (j = 0; j < rhs->c; j++){
            res->m[i][j] = 0;
            for (k = 0; k < lhs->c; k++){
                res->m[i][j] += lhs->m[i][k]*rhs->m[j][k];

            }
        }
    }
    return res;
}

void TCCS_matrix_f32_destroy(TCCSMatrixF32 *mat){
    size_t i;
    for (i = 0; i < mat->r; i++){
        free(mat->m[i]);
    }
    free(mat->m);
    free(mat);
}
