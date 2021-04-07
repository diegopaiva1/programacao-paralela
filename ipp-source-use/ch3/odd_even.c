/* File:    odd_even.c
 *
 * Purpose: Use odd-even transposition sort to sort a list of ints.
 *
 * Compile: gcc -g -Wall -o odd_even odd_even.c
 * Run:     odd_even <n> <g|i>
 *             n:   number of elements in list
 *            'g':  generate list using a random number generator
 *            'i':  user input list
 *
 * Input:   list (optional)
 * Output:  sorted list
 *
 * IPP:     Section 3.7.1 (p. 128) and Section 5.6.2 (pp. 233 and ff.)
 */
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <mpi.h>

/* Keys in the random list in the range 0 <= key < RMAX */
const int RMAX = 100;

void Usage(char* prog_name);
void Get_args(int argc, char* argv[], int* n_p, char* g_i_p);
void Generate_list(int a[], int n);
void Print_list(int a[], int n, int my_rank, int comm_sz, char* title);
void Read_list(int a[], int n);
void Odd_even_sort(int a[], int n, int my_rank, int comm_sz);
int Is_sorted_serial(int a[], int n);
int Is_sorted_parallel(int a[], int n, int my_rank, int comm_sz);

/*-----------------------------------------------------------------*/
int main(int argc, char* argv[]) {
   int my_rank, comm_sz;

   MPI_Init(NULL, NULL);
   MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
   MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);

   int  n;
   char g_i;
   int* a;
   
   Get_args(argc, argv, &n, &g_i);
   a = (int*) malloc(n*sizeof(int));
   if (g_i == 'g') {
      Generate_list(a, n);
      // Print_list(a, n, my_rank, comm_sz, "Before sort");
   } else {
      Read_list(a, n);
   }

   Odd_even_sort(a, n, my_rank, comm_sz);

   // Print_list(a, n, my_rank, comm_sz, "After sort");
   
   free(a);
   return 0;
}  /* main */


/*-----------------------------------------------------------------
 * Function:  Usage
 * Purpose:   Summary of how to run program
 */
void Usage(char* prog_name) {
   fprintf(stderr, "usage:   %s <n> <g|i>\n", prog_name);
   fprintf(stderr, "   n:   number of elements in list\n");
   fprintf(stderr, "  'g':  generate list using a random number generator\n");
   fprintf(stderr, "  'i':  user input list\n");
}  /* Usage */


/*-----------------------------------------------------------------
 * Function:  Get_args
 * Purpose:   Get and check command line arguments
 * In args:   argc, argv
 * Out args:  n_p, g_i_p
 */
void Get_args(int argc, char* argv[], int* n_p, char* g_i_p) {
   if (argc != 3 ) {
      Usage(argv[0]);
      exit(0);
   }
   *n_p = atoi(argv[1]);
   *g_i_p = argv[2][0];

   if (*n_p <= 0 || (*g_i_p != 'g' && *g_i_p != 'i') ) {
      Usage(argv[0]);
      exit(0);
   }
}  /* Get_args */


/*-----------------------------------------------------------------
 * Function:  Generate_list
 * Purpose:   Use random number generator to generate list elements
 * In args:   n
 * Out args:  a
 */
void Generate_list(int a[], int n) {
   int i;

   srandom(0);
   for (i = 0; i < n; i++)
      a[i] = random() % RMAX;
}  /* Generate_list */


/*-----------------------------------------------------------------
 * Function:  Print_list
 * Purpose:   Print the elements in the list
 * In args:   a, n
 */
void Print_list(int a[], int n, int my_rank, int comm_sz, char* title) {
   int i;

   printf("%s:\n", title);
   for (i = 0; i < n; i++)
      printf("%d ", a[i]);
   Is_sorted_parallel(a, n, my_rank, comm_sz) == 1 ? printf("\nSorted") : printf("\nNot Sorted");
   printf("\n\n");
}  /* Print_list */


/*-----------------------------------------------------------------
 * Function:  Read_list
 * Purpose:   Read elements of list from stdin
 * In args:   n
 * Out args:  a
 */
void Read_list(int a[], int n) {
   int i;

   printf("Please enter the elements of the list\n");
   for (i = 0; i < n; i++)
      scanf("%d", &a[i]);
}  /* Read_list */

/*-----------------------------------------------------------------
 * Function:   Is_sorted_parallel
 * Purpose:    Determine whether list 'a' is sorted 
 * In args:    n
 * Out args:   a
 * Return val: 1 if 'a' is sorted, 0 otherwise 
 */
int Is_sorted_parallel(int a[], int n, int my_rank, int comm_sz) {
   int local_n = n/comm_sz;
   int local_start = 0 + my_rank * local_n;
   int local_end = local_start + local_n;
   int local_sorted = 1;
   int global_sorted;

   for (int i = local_start; i < local_end; i++) {
      if (a[i - 1] > a[i]) {
         local_sorted = 0;
         break;
      }
   }

   MPI_Reduce(&local_sorted, &global_sorted, 1, MPI_INT, MPI_MIN, 0, MPI_COMM_WORLD);
   return global_sorted;
} /* Is_sorted_parallel */

/*-----------------------------------------------------------------
 * Function:   Is_sorted_serial
 * Purpose:    Determine whether list 'a' is sorted 
 * In args:    n
 * Out args:   a
 * Return val: 1 if 'a' is sorted, 0 otherwise 
 */
int Is_sorted_serial(int a[], int n) {
   for (int i = 1; i < n; i++)
      if (a[i - 1] > a[i])
         return 0;
   
   return 1;
} /* Is_sorted_serial */

/*-----------------------------------------------------------------
 * Function:     Odd_even_sort
 * Purpose:      Sort list using odd-even transposition sort
 * In args:      n
 * In/out args:  a
 */
void Odd_even_sort(
      int  a[]  /* in/out   */, 
      int  n    /* in       */,
      int my_rank /* in     */, 
      int comm_sz /* in     */) {
   for (int phase = 0; phase < n; phase++) {
      if (phase % 2 == 0) { /* Even phase */
         for (int i = 1; i < n; i += 2) 
            if (a[i-1] > a[i]) {
               int temp = a[i];
               a[i] = a[i-1];
               a[i-1] = temp;
            }
      } else { /* Odd phase */
         for (int i = 1; i < n-1; i += 2)
            if (a[i] > a[i+1]) {
               int temp = a[i];
               a[i] = a[i+1];
               a[i+1] = temp;
            }
      }

      // if (Is_sorted_parallel(a, n, my_rank, comm_sz) == 1)
      //    break;         
      
      if (Is_sorted_serial(a, n) == 1)
         break;                  
   } 
}  /* Odd_even_sort */
