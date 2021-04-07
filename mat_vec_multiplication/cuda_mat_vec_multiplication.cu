#include <stdio.h>
#include <cuda.h>
#include <cuda_runtime.h>

#define THREADS_PER_BLOCK 256
#define MULTIPLICATIONS 4096

/**
 * Multiply square matrix (n x n) by the vector of size n.
 * 
 * 
 * @param mat Input matrix.
 * @param vec Input vector.
 * @param out Output vector.
 * @param n   Dimension.  
 */
__global__ void matrix_vector_multiplication(float *mat, float *vec, float *out, int n) 
{
    int tid = blockIdx.x * blockDim.x + threadIdx.x;

    if (tid < n) {
        float sum = 0;
        
        for (int k = 0; k < MULTIPLICATIONS; k++)
            for (int i = 0; i < n; i++)
                sum += mat[i * n + tid] * vec[i];
        
        out[tid] = sum;            
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
    
    // Host & device inputs and outputs
    float *h_mat, *h_vec, *h_out;
    float *d_mat, *d_vec, *d_out;

    // Allocate host memory
    h_mat = (float*) malloc(sizeof(float) * n * n);
    h_vec = (float*) malloc(sizeof(float) * n);
    h_out = (float*) malloc(sizeof(float) * n);

    // Allocate device memory 
    cudaMalloc((void**) &d_mat, sizeof(float) * n * n);
    cudaMalloc((void**) &d_vec, sizeof(float) * n);
    cudaMalloc((void**) &d_out, sizeof(float) * n);

    // Initialize host matrix
    for (int i = 0; i < n; i++)
        for (int j = 0; j < n; j++)
            h_mat[i * n + j] = 0.05;
    
    // Initialize host vector
    for (int i = 0; i < n; i++)
        h_vec[i] = 0.05;

	// Transfer data from host to device memory
	cudaMemcpy(d_mat, h_mat, sizeof(float) * n * n, cudaMemcpyHostToDevice);
    cudaMemcpy(d_vec, h_vec, sizeof(float) * n, cudaMemcpyHostToDevice);
    
    cudaEvent_t start, finish;
    float elapsed_milliseconds;

    // Use event to calculate time
	cudaEventCreate(&start); 
	cudaEventCreate(&finish);

	cudaEventRecord(start, 0);
    matrix_vector_multiplication<<<n/THREADS_PER_BLOCK + 1, THREADS_PER_BLOCK>>>(d_mat, d_vec, d_out, n);
    cudaEventRecord(finish, 0); 

	cudaEventSynchronize(finish);  // Wait for stop event to complete
	cudaEventElapsedTime(&elapsed_milliseconds, start, finish);  // Calculate the time difference (millisecond level)

    // Transfer data back to host memory
	cudaMemcpy(h_out, d_out, sizeof(float) * n, cudaMemcpyDeviceToHost);

    printf("%f\n", h_out[0]);
    printf("GPU Elapsed time = %.2fs\n", elapsed_milliseconds/1000.0);
    
    // Deallocate device memory
    cudaFree(d_mat);
    cudaFree(d_vec);
    cudaFree(d_vec);

    // Deallocate host memory
    free(h_mat); 
    free(h_vec); 
    free(h_out);

    return 0;
}