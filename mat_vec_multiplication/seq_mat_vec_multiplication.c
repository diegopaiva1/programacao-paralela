#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define MULTIPLICATIONS 1024

/**
 * Multiply square matrix (n x n) by vector of size n.
 * 
 * @param mat Input matrix.
 * @param vec Input vector.
 * @param out Output vector.
 * @param n   Dimension.  
 */
void matrix_vector_multiplication(float *mat, float *vec, float *out, int n) 
{
    for (int i = 0; i < n; i++) {
        out[i] = 0;

        for (int k = 0; k < MULTIPLICATIONS; k++)
            for (int j = 0; j < n; j++)
                out[i] += mat[i * n + j] * vec[j];        
    }
}

int main(int argc, char const *argv[])
{
    if (argc != 2) {
        fprintf(stderr, "usage: %s <n>\n", argv[0]);
        exit(1);
    }

    int n = atoi(argv[1]);

    if (n < 0) {
        fprintf(stderr, "Invalid size\n");
        exit(1);
    } 

    float *mat = malloc(sizeof(float) * n * n);
    float *vec = malloc(sizeof(float) * n);
    float *out = malloc(sizeof(float) * n);

    // Initialize matrix
    for (int i = 0; i < n; i++)
        for (int j = 0; j < n; j++)
            mat[i * n + j] = 0.05;
    
    // Initialize vector
    for (int i = 0; i < n; i++)
        vec[i] = 0.05;
    
    clock_t start = clock();
    matrix_vector_multiplication(mat, vec, out, n);
    clock_t finish = clock();

    printf("%f\n", out[0]);
    printf("Elapsed time = %.2fs\n", (double) (finish - start)/CLOCKS_PER_SEC);
    return 0;
}
