/* ============================= C MeatAxe ==================================
   File:        $Id: matadd.c,v 1.1.1.1 2007/09/02 11:06:17 mringe Exp $
   Comment:     Add two matrices
   --------------------------------------------------------------------------
   (C) Copyright 1997 Michael Ringe, Lehrstuhl D fuer Mathematik,
   RWTH Aachen, Germany  <mringe@math.rwth-aachen.de>
   This program is free software; see the file COPYING for details.
   ========================================================================== */


#include "meataxe.h"

/* --------------------------------------------------------------------------
   Local data
   -------------------------------------------------------------------------- */

MTX_DEFINE_FILE_INFO 


/**
 ** @addtogroup mat
 ** @{
 **/

/**
 ** Sum of two matrices.
 ** This function adds @em src to @em dest, overwriteing the previos value in @em dest.
 ** The matrices must be over the same field and have the  same dimensions.
 ** @return @em dest on success, 0 on error.
 **/

Matrix_t *MatAdd(Matrix_t *dest, const Matrix_t *src)
{
    PTR dp, sp;
    register int n;

    /* Check arguments
       --------------- */
#ifdef DEBUG
    if (!MatIsValid(src) || !MatIsValid(dest))
	return NULL;
#endif
    if (dest->Field != src->Field || dest->Nor != src->Nor || 
	dest->Noc != src->Noc)
	return MTX_ERROR1("%E",MTX_ERR_INCOMPAT), NULL;

    /* Add <src> to <dest>
       ------------------- */
    dp = dest->Data;
    sp = src->Data;
    FfSetField(src->Field);   /* No error checking */
    FfSetNoc(src->Noc);
    for (n = src->Nor; n > 0; --n)
    {
	FfAddRow(dp,sp);
	FfStepPtr(&dp);
	FfStepPtr(&sp);
    }

    /* Delete the pivot table
       ---------------------- */
    Mat_DeletePivotTable(dest);

    return dest;
}

/**
 ** @}
 **/
