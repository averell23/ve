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
#ifndef MATRIXUTILS_H
#define MATRIXUTILS_H

#include <log4cplus/logger.h>
#include <gsl/gsl_vector.h>
#include <gsl/gsl_matrix.h>
#include <gsl/gsl_blas.h>
#include <gsl/gsl_multifit.h>
#include <cv.hpp>
#include <cstdlib>

using namespace log4cplus;
using namespace std;

/**
Utility class for matrix operations and conversions.

@author Daniel Hahn,,,
*/
class MatrixUtils{
public:
    /**
       Prints the contents of a matrix to stdout
    */
    static void printMatrix(gsl_matrix* mat);

    /** 
       Prints the contents of a vector to stdout
    */
    static void printVector(gsl_vector* vec);
    
    /**
       Prints the contents of a float array as a vector.
       
       @param content Array which contains the matrix data. The data
                      is supposed to be order row-after-row, this is
		      the format of the CvMatr32f type in OpenCv
       @param rows    Number of rows
       @param cols    Number of columns
       @param offset  Number of elements in the content field before
                      the actual matrix data. This can be used if the
		      actual matrix is only a part of the content field.
    */
    static void printMatrix(float* content, int offset, int rows, int cols);
    
    /**
      Creates a new copy of the given submatrix. The user is
      responsible for deleting the matrix object after use. A copy
      of the submatrix may be neccessary because some functions (e.g. blas_dgemm
      won't work with submatrix views.
    */
    static gsl_matrix* submatrixCopy(gsl_matrix* source, int top, int left, int height, int width);
};

#endif
