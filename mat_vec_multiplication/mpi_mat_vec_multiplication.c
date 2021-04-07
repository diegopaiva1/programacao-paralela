#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

#define MULTIPLICATIONS 1024

int main(int argc, char const *argv[])
{
    int my_rank, comm_sz, n;
    float *mat, *vec, *out;

    MPI_Init(NULL, NULL);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);

    // If I am master I handle input
    if (my_rank == 0) {
        if (argc != 2) {
            fprintf(stderr, "usage: mpiexec -n <p> %s <n>\n", argv[0]);
            exit(1);
        }

        n = atoi(argv[1]);

        if (n < 0) {
            fprintf(stderr, "Invalid size\n");
            exit(1);
        } 
        else if (n % comm_sz > 0) {
            fprintf(stderr, "n should divide comm_sz\n");
            exit(1);
        }

        mat = malloc(sizeof(float) * n * n);
        vec = malloc(sizeof(float) * n);
        out = malloc(sizeof(float) * n);

        // Initialize matrix
        for (int i = 0; i < n; i++)
            for (int j = 0; j < n; j++)
                mat[i * n + j] = 0.05;
        
        // Initialize vector
        for (int i = 0; i < n; i++)
            vec[i] = 0.05;
    }

    // Send n to other processes
    MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);

    if (my_rank != 0)
        vec = malloc(sizeof(float) * n); 
    
    MPI_Bcast(vec, n, MPI_FLOAT, 0, MPI_COMM_WORLD);

    int local_n = n/comm_sz;
    float *local_mat = malloc(sizeof(float) * local_n * n);
    float *local_out = malloc(sizeof(float) * local_n);
    
    double start = MPI_Wtime();
    MPI_Scatter(mat, local_n * n, MPI_FLOAT, local_mat, local_n * n, MPI_FLOAT, 0, MPI_COMM_WORLD);

    // Compute matrix-vector multiplication
    for (int i = 0; i < local_n; i++) {
        local_out[i] = 0;

        for (int k = 0; k < MULTIPLICATIONS; k++)
            for (int j = 0; j < n; j++)
                local_out[i] += local_mat[i * local_n + j] * vec[j];        
    }

    MPI_Gather(local_out, local_n, MPI_FLOAT, out, local_n, MPI_FLOAT, 0, MPI_COMM_WORLD);
    double finish = MPI_Wtime();

    if (my_rank == 0) {
        printf("%f\n", out[0]);
        printf("With (%d) processes, elapsed time = %.2fs\n", comm_sz, finish - start);
        free(mat);
        free(out);
    }
    
    free(vec);
    free(local_mat);
    free(local_out);
    
    MPI_Finalize();

    return 0;
}
