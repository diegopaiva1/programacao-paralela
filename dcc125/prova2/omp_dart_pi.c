#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <omp.h>

int main(int argc, char* argv[])
{
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <threads> <n>\n", argv[0]);
        exit(1);
    }

    int threads = atoi(argv[1]);
    unsigned long long total_tosses = strtoull(argv[2], NULL, 0), total_circle_hits = 0;

    double start = omp_get_wtime();
    #pragma omp parallel num_threads(threads)
    {
        double x, y, pos;

        // Generate random seed for thread (thread-safe)
        struct drand48_data rand_buffer;
        srand48_r((omp_get_thread_num() + 1) * (omp_get_thread_num() + 13), &rand_buffer);

        #pragma omp for reduction(+: total_circle_hits)
        for (unsigned long long toss = 0; toss < total_tosses; toss++) {
            // Generate random x and y in range [0.0, 1.0).
            drand48_r(&rand_buffer, &x);
            drand48_r(&rand_buffer, &y);

            // Adjust x and y to be in range [-1.0, 1.0).
            x = x * 2.0 - 1.0;
            y = y * 2.0 - 1.0;

            pos = x * x + y * y;

            if (pos <= 1.0)
                total_circle_hits++;         
        }
    } 
    double finish = omp_get_wtime();

    double pi_estimate = (4.0 * total_circle_hits)/total_tosses;
    printf("Pi estimate = %.12e\n", pi_estimate);
    printf("Elapsed time = %.2fs\n", finish - start);

    return 0;
}
