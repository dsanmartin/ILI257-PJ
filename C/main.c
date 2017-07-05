#include <stdio.h>
#include <stdlib.h>
#include "threadpool.h"

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

void blockMult(Matrix *A, Matrix *B, Matrix *C, int start, int end);

void printMatrix(Matrix *M);

double elementMult(Matrix *A, Matrix *B, int row, int col);

void fillMatrix(Matrix *M, int start, int end);

void *matMult(void *param);

void *_th_fillMatrix(void **args);
void *_th_blockMult(void **args);


int main(int argc, char *argv[])
{	
	//Size of matrix
	unsigned int m;
	unsigned int o;
	unsigned int n;

	//Number of threads
	unsigned int n_threads;

	int a, b;

	//Assign values
	m = atoi(argv[1]);
	o = atoi(argv[2]);
	n = atoi(argv[3]);
	n_threads = atoi(argv[4]);

	void *th_arguments[5];

	Matrix *A = createMatrix(m, o);
	Matrix *B = createMatrix(o, n);
	Matrix *C = createMatrix(m ,n);

	
	/*
	unsigned int blockA = A->size % n_threads == 0? A->size / n_threads : A->size / n_threads + 1;
	unsigned int blockB = B->size % n_threads == 0? B->size / n_threads : B->size / n_threads + 1;
	*/

	poolThread *P;

	// Creacion Pool
	if(n_threads > 1) {
		P = createPool(n_threads);
	}


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
		unsigned int blockA = A->size%n_threads == 0? A->size/n_threads : A->size/n_threads + 1;
		unsigned int blockB = B->size%n_threads == 0? B->size/n_threads : B->size/n_threads + 1;
		unsigned int blockC = C->size%n_threads == 0? C->size/n_threads : C->size/n_threads + 1;

		for(int i=0; i < n_threads; i++) 
		{
			
			// Fill matrix A
			a = i*blockA;
			b = (i+1)*blockA;

			th_arguments[0] = (void *)A;
			th_arguments[1] = (void *)&a;
			th_arguments[2] = (void *)&b;
			
			poolSendJob(P, *_th_fillMatrix, th_arguments);

			// Fill matrix B
			a = i*blockB;
			b = (i+1)*blockB;

			th_arguments[0] = (void *)B;
			th_arguments[1] = (void *)&a;
			th_arguments[2] = (void *)&b;

			poolSendJob(P, *_th_fillMatrix, th_arguments);


		}
	}


	// Show result
	printf("Matrix A:\n");
	printMatrix(A);
	printf("\nMatrix B:\n");
	printMatrix(B);
	printf("\nMatrix C:\n");
	printMatrix(C);

	// Liberar memoria 
	delMatrix(A);
	delMatrix(B);
	delMatrix(C);

	return 0;
}


void fillMatrix(Matrix *M, int start, int end)
{
	printf("start: %d\n", start);
	printf("end: %d\n", end);
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

void blockMult(Matrix *A, Matrix *B, Matrix *C, int start, int end)
{
	for( ;(start < end) && (start < C->size); start++)
		C->values[start] = elementMult(A, B, start/A->rows, start%A->rows);
}

void *_th_fillMatrix(void **args) {
	fillMatrix((Matrix *) args[0], *((int *)args[1]), *((int *)args[2]));
	//fillMatrix((Matrix *) args[0], (int) args[1], (int) args[2]);
}

void *_th_blockMult(void **args)
{
	blockMult((Matrix *) args[0], (Matrix *) args[1], (Matrix *) args[2], *((int *)args[3]), *((int *)args[3]));
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

