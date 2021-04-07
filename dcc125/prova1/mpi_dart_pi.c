#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <mpi.h>

/**
 * Generate random double in interval [min, max]
 */
double random_double(double min, double max)
{
    double scale = rand()/(double) RAND_MAX; // [0, 1.0]
    return min + scale * (max - min);        // [min, max]
}

int main(int argc, char* argv[])
{
    // Seed random engine
    srand(time(NULL));

    int my_rank, comm_sz;
    unsigned long long total_tosses = strtoull(argv[1], NULL, 0), local_tosses, total_circle_hits, local_circle_hits = 0;

    MPI_Init(NULL, NULL);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);

    double start = MPI_Wtime();
    MPI_Bcast(&total_tosses, 1, MPI_UNSIGNED_LONG_LONG, 0, MPI_COMM_WORLD);
    local_tosses = total_tosses/comm_sz;

    for (unsigned long long toss = 0; toss < local_tosses; toss++) {
        double x = random_double(-1.0, 1.0);
        double y = random_double(-1.0, 1.0);
        double pos = x*x + y*y;

        if (pos <= 1.0) 
            local_circle_hits++;         
    }

    MPI_Reduce(&local_circle_hits, &total_circle_hits, 1, MPI_UNSIGNED_LONG_LONG, MPI_SUM, 0, MPI_COMM_WORLD);
    double finish = MPI_Wtime();

    if (my_rank == 0) {
        double pi_estimate = (4.0 * total_circle_hits)/total_tosses;
        printf("Pi estimate = %.12e\n", pi_estimate);
        printf("Elapsed time = %.2fs\n", finish - start);
    }

    MPI_Finalize();

    return 0;
}
