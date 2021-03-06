/* ============================= C MeatAxe ==================================
   File:        $Id: mattrace.c,v 1.1.1.1 2007/09/02 11:06:17 mringe Exp $
   Comment:     Trace of a matrix
   --------------------------------------------------------------------------
   (C) Copyright 1997 Michael Ringe, Lehrstuhl D fuer Mathematik,
   RWTH Aachen, Germany  <mringe@math.rwth-aachen.de>
   This program is free software; see the file COPYING for details.
   ========================================================================== */


#include "meataxe.h"

/**
 ** @addtogroup mat
 ** @{
 **/


/**
 ** Trace of a Matrix.
 ** This function calculates the sum of all diagonal elements of a matrix.
 ** Note that the matrix need not be square.
 ** @param mat Pointer to the matrix.
 ** @return Trace of @a mat, @c 255 on error.
 **/

FEL MatTrace(const Matrix_t *mat)
{
    int i;
    PTR x;
    int maxi;
    FEL trace = FF_ZERO;

    /* Check the argument
       ------------------ */
#ifdef DEBUG
    if (!MatIsValid(mat))
	return (FEL)255;
#endif

    maxi = mat->Nor > mat->Noc ? mat->Noc : mat->Nor;
    FfSetField(mat->Field);
    FfSetNoc(mat->Noc);
    for (i = 0, x = mat->Data; i < maxi; ++i, FfStepPtr(&x))
	trace = FfAdd(trace,FfExtract(x,i));
    return trace;
}

/**
 ** @}
 **/
