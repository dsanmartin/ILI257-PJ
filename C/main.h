#define DT 1E-2
#define EPS 1E-2

struct _matrix
{
	unsigned int rows, cols, size;
	double *values;
} typedef Matrix;

Matrix *createMatrix(unsigned int rows, unsigned int cols);
void delMatrix(Matrix *M);
double randomNumber();
void seqfillMatrix(int rows, int cols, double *M);
void seqMatMult(int m, int o, int n, double *A, double *B, double *C);
void blockMult(Matrix *A, Matrix *B, Matrix *C, int start, int end);
void printMatrix(Matrix *M, FILE *f);
double elementMult(Matrix *A, Matrix *B, int row, int col);
//void fillMatrix(Matrix *M, int start, int end);
void chebX(Matrix *x, unsigned int start, unsigned int end); 
void chebMatrix(Matrix *DN, Matrix *x, unsigned int start, unsigned int end);
void *matMult(void *param);
//void *_th_fillMatrix(void **args);
void *_th_blockMult(void **args);
void *_th_chebX(void **args);
void *_th_vectorIC(void **args);
void *_th_chebMatrix(void **args);

//void vectorFunction(Matrix *vector, Matrix *x, void (*f)(double), unsigned int start, unsigned int end);
double initialCondition(double x);

void vectorIC(Matrix *v, Matrix *x, unsigned int start, unsigned int end);
double PDE(double v, double Au);
void vectorPDE(Matrix *v, Matrix *Au, unsigned int start, unsigned int end);