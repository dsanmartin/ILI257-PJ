#define DT 1E-2 // dt for Euler's method
#define EPS 1E-2 // Equation's epsilon
#define T_MAX 100 // Max time for t axis
#define T_PLOT 2 // Plot size

struct _matrix
{
	unsigned int rows, cols, size;
	double *values;
} typedef Matrix;

Matrix *createMatrix(unsigned int rows, unsigned int cols);
void delMatrix(Matrix *M);
double elementMult(Matrix *A, Matrix *B, int row, int col);
void blockMult(Matrix *A, Matrix *B, Matrix *C, int start, int end);
double elementMultCol(Matrix *A, Matrix *B, int row, int col);
void blockMultCol(Matrix *A, Matrix *B, Matrix *C, int start, int end);
void chebX(Matrix *x, unsigned int start, unsigned int end); 
void chebMatrix(Matrix *DN, Matrix *x, unsigned int start, unsigned int end);
double initialCondition(double x);
void vectorIC(Matrix *v, Matrix *x, unsigned int start, unsigned int end);
double PDE(double v, double Au);
void vectorPDE(Matrix *v, Matrix *Au, unsigned int start, unsigned int end);
void setZeroRows(Matrix *U, unsigned int start, unsigned int end);
void *_th_blockMult(void **args);
void *_th_blockMultCol(void **args);
void *_th_chebX(void **args);
void *_th_chebMatrix(void **args);
void *_th_vectorIC(void **args);
void *_th_vectorPDE(void **args);
void *_th_setZeroRows(void **args);
void printMatrix(Matrix *M, FILE *f);