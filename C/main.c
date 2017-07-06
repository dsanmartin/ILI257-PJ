#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "main.h"
#include "threadpool.h"

int main(int argc, char *argv[])
{	
	/* Variables */
	unsigned int N; // Grid size
	unsigned int n_threads; // Number of threads
	void **th_arguments; // Thread function's arguments
	int *a, *b; // Block size manipulators
	double dt, eps; // Equation's parameters


	/* Assign values */
	N = atoi(argv[1]); 
	n_threads = atoi(argv[2]);
	Matrix *v = createMatrix(1, N + 1); // Approximation vector
	Matrix *x = createMatrix(1, N + 1); // Chebyshev x	
	Matrix *DN = createMatrix(N + 1, N + 1); //Chebyshev Matrix
	Matrix *D2N = createMatrix(N + 1, N + 1); //Chebyshev Square Matrix 
	dt = 1e-2; // dt for Euler's method
	eps = 1e-2; // Equation's epsilon


	if(n_threads == 1) /* Serial */
	{

		//fillMatrix(A, 0, A->size);
		//fillMatrix(B, 0, B->size);

		chebX(x, 0, x->size);
		chebMatrix(DN, x, 0, DN->size);
		vectorIC(v, x, 0, v->size);

		/* Matrix multiplication for D2N */
		for(int i=0; i < D2N->rows; i++)
			for(int j=0; j < D2N->cols; j++)
				D2N->values[i*D2N->cols + j] = elementMult(DN, DN, i, j);

	} 
	else /* Parallel */
	{
		/* Pool thread creation */
		poolThread *P = createPool(n_threads);

		/* 
			Compute blocks for parallel algorithm 
			Vectors have the same size and matrix too
		*/
		unsigned int blockVector = x->size%n_threads == 0? x->size/n_threads : x->size/n_threads + 1;
		unsigned int blockMatrix = DN->size%n_threads == 0? DN->size/n_threads : DN->size/n_threads + 1;

		//printf("%d %d\n", blockVector, blockMatrix);
		//exit(0);

		// Fill x vector
		for(int i=0; i < n_threads; i++) 
		{
			
			th_arguments = (void **) malloc(sizeof(void *) * 3);
			a = (int *) malloc(sizeof(int));
			b = (int *) malloc(sizeof(int));
			*a = i*blockVector;
			*b = (i+1)*blockVector;
			th_arguments[0] = (void *) x;
			th_arguments[1] = (void *) a;
			th_arguments[2] = (void *) b;

			poolSendJob(P, *_th_chebX, th_arguments);
		}

		poolWait(P);

		// Fill v vector

		for(int i=0; i < n_threads; i++) 
		{
			th_arguments = (void **) malloc(sizeof(void *) * 4);
			a = (int *) malloc(sizeof(int));
			b = (int *) malloc(sizeof(int));
			*a = i*blockVector;
			*b = (i+1)*blockVector;
			th_arguments[0] = (void *) v;
			th_arguments[1] = (void *) x;
			th_arguments[2] = (void *) a;
			th_arguments[3] = (void *) b;

			poolSendJob(P, *_th_vectorIC, th_arguments);
		}

		poolWait(P);

			
			
			/*
			// Fill matrix A
			th_arguments = (void **) malloc(sizeof(void *) * 3);
			a = (int *) malloc(sizeof(int));
			b = (int *) malloc(sizeof(int));
			*a = i*blockA;
			*b = (i+1)*blockA;
			th_arguments[0] = (void *) A;
			th_arguments[1] = (void *) a;
			th_arguments[2] = (void *) b;


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
			*/

			

		//}
		

		/*
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
		*/
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
	
	printf("x:\n");
	printMatrix(x, stdout);
	printf("v:\n");
	printMatrix(v, stdout);
	printf("DN:\n");
	printMatrix(DN, stdout);
	printf("DN2:\n");
	printMatrix(D2N, stdout);

	// Liberar memoria 
	delMatrix(x);
	delMatrix(v);
	delMatrix(DN);
	delMatrix(D2N);

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
	for( ;(start < end) && (start < M->size); start++)
		M->values[start] = randomNumber();
}

double elementMult(Matrix *A, Matrix *B, int row, int col)
{
	double c = 0;

	for(int i=0; i < A->cols; i++)
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


double initialCondition(double x)
{
	return .53*x + .47*sin(-1.5*M_PI*x); 
}


void vectorIC(Matrix *v, Matrix *x, unsigned int start, unsigned int end)
{
	for( ;(start < end) && (start < v->size); start++)
		v->values[start] = initialCondition(x->values[start]);
}

void chebX(Matrix *x, unsigned int start, unsigned int end) 
{
	for( ;(start < end) && (start < x->size); start++)
		x->values[start] = cos(M_PI*start/(x->size-1));
}

void chebMatrix(Matrix *DN, Matrix *x, unsigned int start, unsigned int end)
{
	for( ;(start < end) && (start < DN->size); start++){
		if (start == 0)
			DN->values[start] = (double) (2*(DN->cols-1)*(DN->cols-1)+ 1)/6;
		else if (start == (DN->size - 1))
			DN->values[start] = (double) -1*(2*(DN->cols-1)*(DN->cols-1)+ 1)/6;
		else
		{
			if (start % (DN->cols+1) == 0)
				DN->values[start] = -x->values[start/x->cols]/(2.0 * (1.0 - x->values[start/x->cols] * x->values[start/x->cols]));					
			else
			{
				int i, j;
				double ci, cj;
				i = start/DN->cols;
				j = start%DN->rows;
				ci = i == 0 || i == (DN->cols - 1) ? 2 : 1;
				cj = j == 0 || j == (DN->rows - 1) ? 2 : 1;
				DN->values[start] = (ci * pow(-1.0, (double) i + j))/(cj * (x->values[i] - x->values[j]));
			}
		}
	}
}

void *_th_chebX(void **args)
{
	chebX((Matrix *) args[0], *((int *)args[1]), *((int *)args[2]));
	free(args[1]);
	free(args[2]);
	free(args);
}

void *_th_vectorIC(void **args)
{
	vectorIC((Matrix *) args[0], (Matrix *) args[1], *((int *)args[2]), *((int *)args[3]));
	free(args[2]);
	free(args[3]);
	free(args);
}

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

