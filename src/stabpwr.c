/* ============================= C MeatAxe ==================================
   File:        $Id: stabpwr.c,v 1.1.1.1 2007/09/02 11:06:17 mringe Exp $
   Comment:     Find the (kernel-)stable power of a matrix.
   --------------------------------------------------------------------------
   (C) Copyright 1999 Michael Ringe, Lehrstuhl D fuer Mathematik,
   RWTH Aachen, Germany  <mringe@math.rwth-aachen.de>
   This program is free software; see the file COPYING for details.
   ========================================================================== */


#include "meataxe.h"

/* --------------------------------------------------------------------------
   Local data
   -------------------------------------------------------------------------- */

MTX_DEFINE_FILE_INFO



/**
!section algo.etc
 ** Stable power of a matrix.
 ** @param mat
    The matrix.
 ** @param pwr
    Stable power.
 ** @param ker
    Kernel of the stable power.
 ** @return
    $0$ on success, $-1$ on error.
!description
    This function takes a matrix $M$ and finds an integer $n>0$ such that 
    $\ker M^n = \ker M^{n+1}$. The matrix, which must be quare, is passed
    in |mat|. |pwr| must be a pointer to a variable receiving the power, 
    $n$. |ker| must be a pointer to a variable of type |Matrix_t *|, 
    where the stable kernel will be stored. Both |pwr| and |ker| may be 
    NULL if the corresponding information is not needed.

    Note that the number $n$ found by |StablePower_()| is not guararanteed 
    to be minimal. In fact, $n$ will always be a power of two since the
    function only examines matrices of the form $M^{2^k}$.

    This function modifies the matrix. To avoid this, use |StablePower()|.
 ** @see StablePower
 **/

int StablePower_(Matrix_t *mat, int *pwr, Matrix_t **ker)

{
    int p;			/* Current power */
    Matrix_t *k1, *k2;		/* Temporary matrices */

    /* Check the arguments.
       -------------------- */
    if (!MatIsValid(mat))
    {
	MTX_ERROR1("mat: %E",MTX_ERR_BADARG);
	return -1;
    }
    if (mat->Nor != mat->Noc)
    {
	MTX_ERROR1("%E",MTX_ERR_NOTSQUARE);
	return -1;
    }

    /* Calculate the stable power.
       --------------------------- */
    p = 1;
    k1 = MatNullSpace(mat);
    if (!k1) return -1;
    if (!MatMul(mat,mat)) return -1;
    k2 = MatNullSpace(mat);
    if (!k2) return -1;
    while (k2->Nor > k1->Nor)
    {
	p *= 2;
	MatFree(k1);
	k1 = k2;
	if (!MatMul(mat,mat)) return -1;
	k2 = MatNullSpace(mat);
    if (!k2) return -1;
    }
    MatFree(k2);

    /* Return the result.
       ------------------ */
    if (ker != NULL)
	*ker = k1;
    else
	MatFree(k1);
    if (pwr != NULL)
	*pwr = p;

    return 0;
}




/**
 ** Stable power of a matrix.
 ** @param mat
    The matrix.
 ** @param pwr
    Stable power.
 ** @param ker
    Kernel of the stable power.
 ** @return
    $0$ on success, $-1$ on error.
!description
    This function works like |StablePower_()|, but it does not modify
    the matrix  in |mat|. This means, of course, that a temporary copy
    of the matrix is created.

 ** @see StablePower_
 **/

int StablePower(const Matrix_t *mat, int *pwr, Matrix_t **ker)

{
    int rc;
    Matrix_t *tmp;

    tmp = MatDup(mat);
    if (tmp == NULL)
    {
	MTX_ERROR1("mat: %E",MTX_ERR_BADARG);
	return -1;
    }
    rc = StablePower_(tmp,pwr,ker);
    MatFree(tmp);
    return rc;
}

