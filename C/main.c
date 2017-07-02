#include <stdio.h>
#include <stdlib.h>
/* #include "threadpool.h" */

#define N_MIN 0
#define N_MAX 1

struct _matrix
{
	unsigned int rows, cols, size;
	double *values;
} typedef Matrix;

Matrix *createMatrix(unsigned int rows, unsigned int cols)
{
	Matrix *M = (Matrix *) malloc(sizeof(Matrix));
	M->rows = rows;
	M->cols = cols;
	M->size = rows*cols;
	M->values = (double *) malloc(sizeof(double)*rows*cols);
};

void delMatrix(Matrix *M)
{
	free(M->values);
	free(M);
};

double randomNumber();

void seqfillMatrix(int rows, int cols, double *M);

void seqMatMult(int m, int o, int n, double *A, double *B, double *C);

void printMatrix(Matrix *M);

double elementMult(Matrix *A, Matrix *B, int row, int col);

void fillMatrix(Matrix *M, int start, int end);

void *matMult(void *param);


int main(int argc, char *argv[])
{	
	//Size of matrix
	unsigned int m;
	unsigned int o;
	unsigned int n;

	//Number of threads
	unsigned int n_threads;

	//Assign values
	m = atoi(argv[1]);
	o = atoi(argv[2]);
	n = atoi(argv[3]);
	n_threads = atoi(argv[4]);

	// Threads 
	//pthread_t *thread_r1; // Thread for random matrix 1
	//pthread_t *thread_r2; // Thread for random matrix 2
	//pthread_t *thread_m;  // Thread for matrix multiplication

	/*
	double *A = (double *) malloc(sizeof(double)*m*o); // First random matrix
	double *B = (double *) malloc(sizeof(double)*o*n); // Second random matrix
	double *C = (double *) malloc(sizeof(double)*m*n); // Result matrix
	*/

	Matrix *A = createMatrix(m, o);
	Matrix *B = createMatrix(o, n);
	Matrix *C = createMatrix(m ,n);

	unsigned int blockA = A->size % n_threads == 0? A->size / n_threads : A->size / n_threads + 1;
	unsigned int blockB = B->size % n_threads == 0? B->size / n_threads : B->size / n_threads + 1;


	if(n_threads == 1) /* Sequential */
	{

		fillMatrix(A, 0, A->size);
		fillMatrix(B, 0, B->size);

		for(int i=0; i < C->rows; i++)
			for(int j=0; j < C->cols; j++)
				C->values[i*C->cols + j] = elementMult(A, B, i, j);
	} 
	else /* Parallel */
	{
		int block = (C->size%n_threads == 0)? C->size/n_threads : C->size/n_threads + 1;

		for(int i=0; i < n_threads; i++) 
		{

			fillMatrix(A, i*block, (i+1)*block);
			fillMatrix(B, i*block, (i+1)*block);

			// TODO paralelizar

		}
	}


	// Show result
	/*
	printf("Matrix A:\n");
	printMatrix(A);
	printf("\nMatrix B:\n");
	printMatrix(B);
	printf("\nMatrix C:\n");
	printMatrix(C);
	*/

	// Liberar memoria 
	delMatrix(A);
	delMatrix(B);
	delMatrix(C);

	return 0;
}


void fillMatrix(Matrix *M, int start, int end)
{
	for( ;(start < end) && (start < M->size); start++)
		M->values[start] = randomNumber();
}

double elementMult(Matrix *A, Matrix *B, int row, int col)
{
	int i, j;
	double c = 0;

	for(i=0; i < A->cols; i++)
		c += A->values[A->cols*row + i] * B->values[B->cols*i + col];

	return c;
}

void printMatrix(Matrix *M)
{
	int i, j;
	for(i = 0; i < M->rows; i++) {    /* Iterate of each row */
	    for(j = 0; j < M->cols; j++) {  /* In each row, go over each col element  */
	        printf("%lf ", M->values[i*M->cols + j]); /* Print each row element */
	    }
	    printf("\n");
	}
}

double randomNumber()
{
    double f = (double)rand() / RAND_MAX;
    return N_MIN + f * (N_MAX - N_MIN);
}

