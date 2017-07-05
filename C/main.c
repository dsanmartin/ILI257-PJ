#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "main.h"
#include "threadpool.h"

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

	void **th_arguments;
	int *a, *b;

	Matrix *A = createMatrix(m, o);
	Matrix *B = createMatrix(o, n);
	Matrix *C = createMatrix(m ,n);

	// Chebyshev x
	Matrix *x = createMatrix(1, n + 1);

	//Chebyshev Matrix
	Matrix *Dx = createMatrix(n + 1, n+ 1);

	poolThread *P;

	if(n_threads == 1) /* Sequential */
	{

		fillMatrix(A, 0, A->size);
		fillMatrix(B, 0, B->size);

		chebX(x, 0, x->size);

		for(int i=0; i < C->rows; i++)
			for(int j=0; j < C->cols; j++)
				C->values[i*C->cols + j] = elementMult(A, B, i, j);
	} 
	else /* Parallel */
	{
		unsigned int blockA = A->size%n_threads == 0? A->size/n_threads : A->size/n_threads + 1;
		unsigned int blockB = B->size%n_threads == 0? B->size/n_threads : B->size/n_threads + 1;
		unsigned int blockC = C->size%n_threads == 0? C->size/n_threads : C->size/n_threads + 1;

		unsigned int blockX = x->size%n_threads == 0? x->size/n_threads : x->size/n_threads + 1;

		P = createPool(n_threads);

		for(int i=0; i < n_threads; i++) 
		{

			
			// Fill matrix A
			th_arguments = (void **) malloc(sizeof(void *) * 3);
			a = (int *) malloc(sizeof(int));
			b = (int *) malloc(sizeof(int));
			*a = i*blockA;
			*b = (i+1)*blockA;
			th_arguments = (void **) malloc(sizeof(void *) * 3);
			th_arguments[0] = (void *) A;
			th_arguments[1] = (void *) a;
			th_arguments[2] = (void *) b;


			/*
			th_arguments[0] = (void *)A;
			th_arguments[1] = (void *) malloc(sizeof(int));
			th_arguments[2] = (void *) malloc(sizeof(int));
			memset(th_arguments[1], i*blockA, 1);
			memset(th_arguments[2], (i+1)*blockA, 1);
			*/


			poolSendJob(P, *_th_fillMatrix, th_arguments);

			// Fill matrix B
			a = (int *) malloc(sizeof(int));
			b = (int *) malloc(sizeof(int));
			*a = i*blockB;
			*b = (i+1)*blockB;
			th_arguments = (void **) malloc(sizeof(void *) * 3);
			th_arguments[0] = (void *) B;
			th_arguments[1] = (void *) a;
			th_arguments[2] = (void *) b;

			poolSendJob(P, *_th_fillMatrix, th_arguments);

			
			/*
			// Fill x
			a = (int *) malloc(sizeof(int));
			b = (int *) malloc(sizeof(int));
			*a = i*blockX;
			*b = (i+1)*blockX;
			th_arguments = (void **) malloc(sizeof(void *) * 3);
			th_arguments[0] = (void *) x;
			th_arguments[1] = (void *) a;
			th_arguments[2] = (void *) b;

			poolSendJob(P, *_th_chebX, th_arguments);
			*/

		}
		poolWait(P);

		for(int i=0; i < n_threads; i++) 
		{
			a = (int *) malloc(sizeof(int));
			b = (int *) malloc(sizeof(int));
			*a = i*blockC;
			*b = (i+1)*blockC;
			th_arguments = (void **) malloc(sizeof(void *) * 5);
			th_arguments[0] = (void *) A;
			th_arguments[1] = (void *) B;
			th_arguments[2] = (void *) C;
			th_arguments[3] = (void *) a;
			th_arguments[4] = (void *) b;

			poolSendJob(P, *_th_blockMult, th_arguments);
		}

		poolWait(P);
	}

	// Show result

	/*
	printf("Matrix A:\n");
	printMatrix(A, stdout);
	printf("\nMatrix B:\n");
	printMatrix(B, stdout);
	printf("\nMatrix C:\n");
	printMatrix(C, stdout);
	*/
	
	//printMatrix(x);

	// Liberar memoria 
	delMatrix(A);
	delMatrix(B);
	delMatrix(C);

	return 0;
}

Matrix *createMatrix(unsigned int rows, unsigned int cols)
{
	Matrix *M = (Matrix *) malloc(sizeof(Matrix));
	M->rows = rows;
	M->cols = cols;
	M->size = rows*cols;
	M->values = (double *) malloc(sizeof(double)*rows*cols);
}

void delMatrix(Matrix *M)
{
	free(M->values);
	free(M);
}

void fillMatrix(Matrix *M, int start, int end)
{
	//printf("start: %d\n", start);
	//printf("end: %d\n", end);
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
	//printf("%d %d\n", start, end);
	for( ;(start < end) && (start < C->size); start++)
		C->values[start] = elementMult(A, B, start/A->rows, start%A->rows);
}

void *_th_fillMatrix(void **args) 
{
	fillMatrix((Matrix *) args[0], *((int *)args[1]), *((int *)args[2]));
	free(args[1]);
	free(args[2]);
	free(args);
}

void *_th_blockMult(void **args)
{
	blockMult((Matrix *) args[0], (Matrix *) args[1], (Matrix *) args[2], *((int *)args[3]), *((int *)args[4]));
	free(args[3]);
	free(args[4]);
	free(args);
}

void chebX(Matrix *x, unsigned int start, unsigned int end) 
{
	for( ;(start < end) && (start < x->size); start++)
		x->values[start] = cos(M_PI*start/(x->size-1));
}

void *_th_chebX(void **args)
{
	chebX((Matrix *) args[0], *((int *)args[1]), *((int *)args[2]));
	free(args[1]);
	free(args[2]);
	free(args);
}

/*void chebMatrix(Matrix *Dx, , unsigned int start, unsigned int end)
{
	for( ;(start < end) && (start < x->size); start++){
		if (start == 0 && )
		x->values[start] = cos(M_PI*start/(x->size-1));
	}
}*/

void printMatrix(Matrix *M, FILE *f)
{
	int i, j;
	for(i = 0; i < M->rows; i++) {    /* Iterate of each row */
	    for(j = 0; j < M->cols; j++) {  /* In each row, go over each col element  */
	        fprintf(f, "%lf, ", M->values[i*M->cols + j]); /* Print each row element */
	    }
	    fprintf(f, "\n");
	}
}

double randomNumber()
{	
    double f = (double)rand() / RAND_MAX;
    return N_MIN + f * (N_MAX - N_MIN);
}

