#include "tccsmath.h"

void swapi32(int *x, int *y){
    *x ^= *y;
    *y ^= *x;
    *x ^= *y;
}

TCCSMatrixF32* TCCS_matrix_f32_create(size_t r, size_t c){
    TCCSMatrixF32 *mat = (TCCSMatrixF32*) malloc(sizeof(TCCSMatrixF32));
    if (mat){
        mat->r = r;
        mat->c = c;
        mat->m = (float**) malloc(sizeof(float*) * r);
        if (mat->m){
            void *end;
            for (end = &mat->m[r]; mat->m != end; mat->m++){
                (*mat->m) = (float*) malloc(sizeof(float) * c);
            }
        }
    }
    if (mat != NULL && r != 0 && c != 0 && mat->m == NULL){
        TCCS_matrix_f32_destroy(mat);
        mat = NULL;
    }
    return mat;
}

void TCCS_matrix_f32_destroy(TCCSMatrixF32 *mat){
    free(mat->m);
    free(mat);
}
