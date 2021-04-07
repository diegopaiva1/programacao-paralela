/**
 * TENTATIVA de paralelização da função Is_sorted
 * 
 * A estratégia consiste basicamente nos passos:
 * 1. Dividir os dados do array uniformemente entre os cores;
 * 2. Cada core verifica se sua sequência está ordenada;
 * 3. Se todas seqs. estiverem ordenadas, diga que o array tá ordenado.
 *  
 * Contudo esta estratégia não tá totalmente correta, pq cada sequência indivdual
 * estar ordenada não significa necessariamente que o array todo ta ordenado. 
 * Seria necessário fazer um overlap entre os limites de cada sequência pra garantir a corretude.
 * 
 * Além disso, a performance está estranha, pq rodando com 1 core é mais rápido do que rodando com 2 ou 4.
 * Não sei explicar o pq disso. 
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <mpi.h>

#define MAX_KEY 1

int main(int argc, char* argv[])
{
    int my_rank, comm_sz;

    MPI_Init(NULL, NULL);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);

    srand(time(NULL));

    int *v;
    int n;
    int ROOT = 0;

    if (my_rank == ROOT)  {
        if (argc != 2) {
            fprintf(stderr, "Usage: mpiexec -n <p> %s <N>\n", argv[0]);
            exit(1);
        }
        else {
            n = atoi(argv[1]);

            if (n % comm_sz > 0) {
                fprintf(stderr, "n should divide comm_sz\n");
                exit(1);
            }

            v = malloc(sizeof(int) * n);
            
            for (int i = 0; i < n; i++)
                v[i] = rand() % (MAX_KEY + 1);
        }
    }

    MPI_Bcast(&n, 1, MPI_INT, ROOT, MPI_COMM_WORLD);

    int local_n = n/comm_sz;
    int *local_v = malloc(sizeof(int) * local_n);

    double start = MPI_Wtime();
    MPI_Scatter(v, local_n, MPI_INT, local_v, local_n, MPI_INT, ROOT, MPI_COMM_WORLD);
    int local_sorted = 1, global_sorted;

    for (int i = 1; i < local_n; i++) {
        if (local_v[i - 1] > local_v[i]) {
            local_sorted = 0;
            break;
        }
    }

    MPI_Reduce(&local_sorted, &global_sorted, 1, MPI_INT, MPI_MIN, ROOT, MPI_COMM_WORLD);
    double finish = MPI_Wtime();
    
    if (my_rank == ROOT) {
        printf("v sorted? %d\n", global_sorted);
        printf("Elapsed time = %.2fms\n", (finish - start) * 1000.0);
        free(v);
    }
    
    free(local_v);
    MPI_Finalize();
    return 0;
}