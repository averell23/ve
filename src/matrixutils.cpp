/***************************************************************************
 *   Copyright (C) 2003 by Daniel Hahn,,,                                  *
 *   daniel@81pc04                                                         *
 *                                                                         *
 *   Permission is hereby granted, free of charge, to any person obtaining *
 *   a copy of this software and associated documentation files (the       *
 *   "Software"), to deal in the Software without restriction, including   *
 *   without limitation the rights to use, copy, modify, merge, publish,   *
 *   distribute, sublicense, and/or sell copies of the Software, and to    *
 *   permit persons to whom the Software is furnished to do so, subject to *
 *   the following conditions:                                             *
 *                                                                         *
 *   The above copyright notice and this permission notice shall be        *
 *   included in all copies or substantial portions of the Software.       *
 *                                                                         *
 *   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,       *
 *   EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF    *
 *   MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.*
 *   IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR     *
 *   OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, *
 *   ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR *
 *   OTHER DEALINGS IN THE SOFTWARE.                                       *
 ***************************************************************************/
#include "matrixutils.h"

void MatrixUtils::printMatrix(gsl_matrix* mat) {
	for (int i=0 ; i<mat->size1 ; i++) {
		for (int j=0 ; j<mat->size2 ; j++) {
			cout << gsl_matrix_get(mat, i, j) << "	";
		}
		cout << endl;
	}
}

void MatrixUtils::printVector(gsl_vector* vec) {
	for (int i=0 ; i<vec->size ; i++) {
		cout << gsl_vector_get(vec, i) << endl;
	}
}

void MatrixUtils::printMatrix(float* content, int offset, int rows, int cols) {
    for (int i=0 ; i<rows ; i++) {
		for (int j=0 ; j<cols ; j++) {
			cout << content[(i*cols) + j + offset] << "	";
		}
		cout << endl;
	}
}

gsl_matrix* MatrixUtils::submatrixCopy(gsl_matrix* source, int top, int left, int height, int width) {
	gsl_matrix* sub = &gsl_matrix_submatrix(source, top, left, height, width).matrix;
	gsl_matrix* retVal = gsl_matrix_alloc(height, width);
	gsl_matrix_memcpy(retVal, sub);
	return retVal;
}

void MatrixUtils::rotate(gsl_matrix* matrix, double alpha, double beta, double gamma) {
    if ((matrix->size1 != 4) || (matrix->size2 != 4)) return; // FIXME: Error handling?
    gsl_matrix* rot_tmp = gsl_matrix_alloc(4,4);
    gsl_matrix* mat_tmp = gsl_matrix_alloc(4,4);
    double alpha_rad = alpha * M_PI/180.0;
    double beta_rad = beta * M_PI/180.0;
    double gamma_rad = gamma * M_PI/180.0;
    // x rotation
    float sine = sin(alpha_rad);
    float cosine = cos(alpha_rad);
    gsl_matrix_set_identity(rot_tmp);
    gsl_matrix_set(rot_tmp, 1, 1, cosine);
    gsl_matrix_set(rot_tmp, 1, 2, sine);
    gsl_matrix_set(rot_tmp, 2, 1, -sine);
    gsl_matrix_set(rot_tmp, 2, 2, cosine);
    gsl_blas_dgemm(CblasNoTrans, CblasNoTrans, 1.0, matrix, rot_tmp, 0.0, mat_tmp);
    // y rotation
    sine = sin(beta_rad);
    cosine = cos(beta_rad);
    gsl_matrix_set_identity(rot_tmp);
    gsl_matrix_set(rot_tmp, 0, 0, cosine);
    gsl_matrix_set(rot_tmp, 0, 2, -sine);
    gsl_matrix_set(rot_tmp, 2, 0, sine);
    gsl_matrix_set(rot_tmp, 2, 2, cosine);
    gsl_blas_dgemm(CblasNoTrans, CblasNoTrans, 1.0, mat_tmp, rot_tmp, 0.0, matrix);
    // z rotation
    sine = sin(gamma_rad);
    cosine = cos(gamma_rad);
    gsl_matrix_set_identity(rot_tmp);
    gsl_matrix_set(rot_tmp, 0, 0, cosine);
    gsl_matrix_set(rot_tmp, 0, 1, sine);
    gsl_matrix_set(rot_tmp, 1, 0, -sine);
    gsl_matrix_set(rot_tmp, 1, 1, cosine);
    gsl_blas_dgemm(CblasNoTrans, CblasNoTrans, 1.0, matrix, rot_tmp, 0.0, mat_tmp);


    gsl_matrix_memcpy(matrix, mat_tmp);

    gsl_matrix_free(rot_tmp);
    gsl_matrix_free(mat_tmp);

}
