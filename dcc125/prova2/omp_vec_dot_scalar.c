#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <time.h>

int main(int argc, char* argv[]) 
{
    if (argc != 4) {
        fprintf(stderr, "Usage: %s <threads> <n> <scalar>\n", argv[0]);
        exit(1);
    }

    int thread_count = atoi(argv[1]);
    unsigned long long n = strtoull(argv[2], NULL, 0);
    double scalar = atof(argv[3]);

    if (thread_count < 0 || thread_count > omp_get_max_threads() || n <= 0) {
        fprintf(stderr, "Invalid argument(s)\n");
        exit(1);
    }

    double *vec1 = malloc(sizeof(double) * n);
    double *vec2 = malloc(sizeof(double) * n);

    #pragma omp parallel num_threads(thread_count)
    {
        // Generate random seed for thread (thread-safe)
        struct drand48_data rand_buffer;
        srand48_r((omp_get_thread_num() + 1) * (omp_get_thread_num() + 13), &rand_buffer);
        
        #pragma omp for
        for (unsigned long long i = 0; i < n; i++)
            drand48_r(&rand_buffer, &vec1[i]);

        #pragma omp for
        for (unsigned long long i = 0; i < n; i++)
            drand48_r(&rand_buffer, &vec2[i]);
    }

    double start = omp_get_wtime();
    double global_result = 0;

    #pragma omp parallel for num_threads(thread_count) reduction(+: global_result)
    for (unsigned long long i = 0; i < n; i++)
        for (int k = 0; k < 100; k++)
            global_result += vec1[i] * vec2[i];   

    global_result *= scalar;
    
    double finish = omp_get_wtime();
    
    printf("Result = %.3f\n", global_result);
    printf("Elapsed time = %.2fs\n", finish - start);
    
    free(vec1);    
    free(vec2);

    return 0;
}
