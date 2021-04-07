#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

#define MULTIPLICATIONS 4096

/**
 * Multiply square matrix (n x n) by the vector vector of size n.
 * 
 * @param mat          Input matrix.
 * @param vec          Input vector.
 * @param out          Output vector.
 * @param n            Dimension.  
 * @param thread_count Number of threads to run.
 */
void matrix_vector_multiplication(float *mat, float *vec, float *out, int n, int thread_count) 
{
    #pragma omp parallel for num_threads(thread_count)
    for (int i = 0; i < n; i++) {
        // To avoid false-sharing
        float sum = 0; 

        for (int k = 0; k < MULTIPLICATIONS; k++)
            for (int j = 0; j < n; j++)
                sum += mat[i * n + j] * vec[j];
        
        out[i] = sum;
    }
}

int main(int argc, char const *argv[])
{
    if (argc != 3) {
        fprintf(stderr, "usage: %s <thread_count> <n>\n", argv[0]);
        exit(1);
    }

    int thread_count = atoi(argv[1]), n = atoi(argv[2]);

    if (thread_count > omp_get_max_threads()) {
        thread_count = omp_get_max_threads();
    }
    else if (n < 0 || thread_count <= 0) {
        fprintf(stderr, "Invalid argument(s)\n");
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
    
    double start = omp_get_wtime();
    matrix_vector_multiplication(mat, vec, out, n, thread_count);
    double finish = omp_get_wtime();
        
    printf("%f\n", out[0]);
    printf("With (%d) threads, elapsed time = %.2fs\n", thread_count, finish - start);
    return 0;
}
