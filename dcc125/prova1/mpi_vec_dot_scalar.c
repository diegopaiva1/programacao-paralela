#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <time.h>

#define MAX_KEY 1

int main(int argc, char* argv[]) 
{
    srand48(time(NULL));

    int my_rank, comm_sz;
    unsigned long long n, local_n, scalar;
    double *vec1, *vec2;

    MPI_Init(NULL, NULL);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);

    // If I am master, I handle input
    if (my_rank == 0) {
        if (argc != 3) {
            fprintf(stderr, "Usage: mpiexec -n <p> %s <N> <scalar>\n", argv[0]);
            return 1;
        }

        n = strtoull(argv[1], NULL, 0);
        scalar = strtoull(argv[2], NULL, 0); 

        if (n <= 0) {
            fprintf(stderr, "Invalid argument(s)\n");
            exit(1);
        } 
        else if (n % comm_sz > 0) {
            fprintf(stderr, "n should divide comm_sz\n");
            exit(1);
        }

        vec1 = malloc(sizeof(double) * n);
        vec2 = malloc(sizeof(double) * n);

        for (int i = 0; i < n; i++)
            vec1[i] = drand48();
        
        for (int i = 0; i < n; i++)
            vec2[i] = drand48();
    }

    // Send n and scalar to other process
    MPI_Bcast(&n, 1, MPI_UNSIGNED_LONG_LONG, 0, MPI_COMM_WORLD);
    MPI_Bcast(&scalar, 1, MPI_UNSIGNED_LONG_LONG, 0, MPI_COMM_WORLD);

    local_n = n/comm_sz;
    double *my_vec1 = malloc(sizeof(double) * local_n);
    double *my_vec2 = malloc(sizeof(double) * local_n);

    double start = MPI_Wtime();
    MPI_Scatter(vec1, local_n, MPI_DOUBLE, my_vec1, local_n, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    MPI_Scatter(vec2, local_n, MPI_DOUBLE, my_vec2, local_n, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    double local_result = 0, global_result = 0;

    for (unsigned long long i = 0; i < local_n; i++)
        local_result += my_vec1[i] * my_vec2[i];
    
    local_result *= scalar;
    MPI_Reduce(&local_result, &global_result, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);
    double finish = MPI_Wtime();
    
    if (my_rank == 0) {
        printf("Result = %.3f\n", global_result);
        printf("Elapsed time = %.2f\n", finish - start);
        free(vec1);
        free(vec2);
    }
    
    free(my_vec1);
    free(my_vec2);
    
    MPI_Finalize();

    return 0;
}
