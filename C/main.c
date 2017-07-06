#include <stdio.h>
#include <stdlib.h>
#include <math.h>
//#include <string.h>
#include "main.h"
#include "threadpool.h"

int main(int argc, char *argv[])
{	
	/* Variables */
	unsigned int N; // Grid size
	unsigned int n_threads; // Number of threads
	void **th_arguments; // Thread function's arguments
	int *a, *b; // Block size manipulators
	double tt; // Equation's parameters dt, eps,
	unsigned int tmax, tplot, nplots, plotgap;


	/* Assign values */
	N = atoi(argv[1]); 
	n_threads = atoi(argv[2]);
	Matrix *v = createMatrix(1, N + 1); // Approximation vector
	Matrix *x = createMatrix(1, N + 1); // Chebyshev x	
	Matrix *DN = createMatrix(N + 1, N + 1); //Chebyshev Matrix
	Matrix *D2N = createMatrix(N + 1, N + 1); //Chebyshev Square Matrix 
	//dt = 1e-2; // dt for Euler's method
	//eps = 1e-2; // Equation's epsilon
	tmax = 100; // Max time for t axis
	tplot = 2; // Plot size
	nplots = (int) round(tmax/tplot);
	plotgap = (int) round(tplot/DT);
	Matrix *U = createMatrix(nplots + 1, N + 1); // Matrix with approximation
	Matrix *t = createMatrix(1, nplots + 1); // t axis vector
	Matrix *tv = createMatrix(1, N + 1); // Temporal approximation
	Matrix *Au = createMatrix(1, N + 1); // Temporal matrix dot vector D2N.v 
	tt = 0.0; // Temporal time for euler method 

	if(n_threads == 1) /* Serial */
	{
		chebX(x, 0, x->size);
		chebMatrix(DN, x, 0, DN->size);
		vectorIC(v, x, 0, v->size);

		t->values[0] = tt;

		for (int w=0; w < v->cols; w++)
			U->values[w] = v->values[w];

		blockMult(DN, DN, D2N, 0, D2N->size);

		// Remove first and last rows to force border conditions
		for(int w=0; w < D2N->cols; w++)
		{
			D2N->values[w] = 0.0;
			D2N->values[D2N->cols*N + w] = 0.0;
		}

		/* Euler method */
		for (int i=1; i <= nplots; i++) 
		{
			for (int j=0; j < plotgap; j++) 
			{
				tt += DT;
				//memcpy(tv, v, sizeof(int)*(N+1));
				for (int w=0; w < v->cols; w++)
					tv->values[w] = v->values[w];

				/* Matrix multiplication for Au */
				//for(int r=0; r < Au->rows; r++)
				//	for(int s=0; s < Au->cols; s++)
				//		Au->values[r*Au->cols + s] = elementMult(D2N, tv, r, s);

				blockMult(D2N, tv, Au, 0, Au->size);

				//printMatrix(Au, stdout);

				vectorPDE(tv, Au, 0, tv->size);

				for (int w=0; w < v->cols; w++)
					v->values[w] = tv->values[w];

			}


			for (int w=0; w < v->cols; w++)
				U->values[U->cols*i + w] = v->values[w];

			t->values[i] = tt;
		}

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


		/* Fill x vector */
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

		/* Fill v vector */
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
		
		/* Create Chebyshev matrix */
		for(int i=0; i < n_threads; i++) 
		{
			th_arguments = (void **) malloc(sizeof(void *) * 4);
			a = (int *) malloc(sizeof(int));
			b = (int *) malloc(sizeof(int));
			*a = i*blockMatrix;
			*b = (i+1)*blockMatrix;
			th_arguments[0] = (void *) DN;
			th_arguments[1] = (void *) x;
			th_arguments[2] = (void *) a;
			th_arguments[3] = (void *) b;

			poolSendJob(P, *_th_chebMatrix, th_arguments);
		}

		poolWait(P);

		/* Create D2N matrix, D2.D2 */
		for(int i=0; i < n_threads; i++) 
		{
			a = (int *) malloc(sizeof(int));
			b = (int *) malloc(sizeof(int));
			*a = i*blockMatrix;
			*b = (i+1)*blockMatrix;
			th_arguments = (void **) malloc(sizeof(void *) * 5);
			th_arguments[0] = (void *) DN;
			th_arguments[1] = (void *) DN;
			th_arguments[2] = (void *) D2N;
			th_arguments[3] = (void *) a;
			th_arguments[4] = (void *) b;

			poolSendJob(P, *_th_blockMult, th_arguments);
		}

		poolWait(P);
		
	}

	// Show result
	/*
	printf("x:\n");
	printMatrix(x, stdout);
	printf("v:\n");
	printMatrix(v, stdout);
	printf("DN:\n");
	printMatrix(DN, stdout);
	printf("DN2:\n");
	printMatrix(D2N, stdout);
	
	printf("U:\n");
	printMatrix(U, stdout);

	printf("tv\n");
	printMatrix(tv, stdout);
	*/


	FILE *ft = fopen("CSV/t.csv", "w");
	FILE *fx = fopen("CSV/x.csv", "w");
	FILE *fu = fopen("CSV/U.csv", "w");
	printMatrix(t, ft);
	printMatrix(x, fx);
	printMatrix(U, fu);
	

	// Liberar memoria 
	delMatrix(x);
	delMatrix(v);
	delMatrix(DN);
	delMatrix(D2N);
	delMatrix(U);
	delMatrix(t);
	delMatrix(tv);
	delMatrix(Au);

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

double elementMult(Matrix *A, Matrix *B, int row, int col)
{
	double c = 0;

	for(int i=0; i < A->cols; i++)
		c += A->values[A->cols*row + i] * B->values[B->cols*i + col];

	return c;
}

void blockMult(Matrix *A, Matrix *B, Matrix *C, int start, int end)
{
	for( ;(start < end) && (start < C->size); start++)
		C->values[start] = elementMult(A, B, start/A->rows, start%A->rows);
}

/*
void *_th_fillMatrix(void **args) 
{
	fillMatrix((Matrix *) args[0], *((int *)args[1]), *((int *)args[2]));
	free(args[1]);
	free(args[2]);
	free(args);
}
*/

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

void *_th_chebMatrix(void **args)
{
	chebMatrix((Matrix *) args[0], (Matrix *) args[1], *((int *)args[2]), *((int *)args[3]));
	free(args[2]);
	free(args[3]);
	free(args);
}


double PDE(double v, double Au)
{
	return v + DT*(EPS * Au + v - v*v*v);
}

void vectorPDE(Matrix *v, Matrix *Au, unsigned int start, unsigned int end)
{
	for( ;(start < end) && (start < v->size); start++)
		v->values[start] = PDE(v->values[start], Au->values[start]);
}


void printMatrix(Matrix *M, FILE *f)
{
	int i, j;
	for(i = 0; i < M->rows; i++) {    /* Iterate of each row */
	    for(j = 0; j < M->cols; j++) {  /* In each row, go over each col element  */
			if(j == (M->cols -1))
				fprintf(f, "%lf ", M->values[i*M->cols + j]);	
			else
	        	fprintf(f, "%lf, ", M->values[i*M->cols + j]); /* Print each row element */
	    }
	    fprintf(f, "\n");
	}
}
