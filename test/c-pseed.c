/* ============================= C MeatAxe ==================================
   File:        $Id: c-pseed.c,v 1.1.1.1 2007/09/02 11:06:17 mringe Exp $
   Comment:     Check functions for matrices.
   --------------------------------------------------------------------------
   (C) Copyright 1998 Michael Ringe, Lehrstuhl D fuer Mathematik,
   RWTH Aachen, Germany  <mringe@math.rwth-aachen.de>
   This program is free software; see the file COPYING for details.
   ========================================================================== */


#include "meataxe.h"
#include "check.h"
#include "c-pseed.h"

#include <stdlib.h>
#include <stdio.h>

/*MTX_DEFINE_FILE_INFO*/


/* --------------------------------------------------------------------------
   TestSeed() - Matrix inversion
   -------------------------------------------------------------------------- */


void TestSeed(unsigned flags)
{
    Matrix_t *basis, *vecs, *cmp;
    PTR dummy;
    long n;
    int i;

    SelectField(3);
    basis = MatId(3,3);
    vecs = MatAlloc(3,13,3);
    cmp = MkMat(13,3,
	1,0,0, 	0,1,0, 	1,1,0, 	2,1,0, 	0,0,1,
	1,0,1, 	2,0,1, 	0,1,1, 	1,1,1, 	2,1,1,
	0,2,1, 	1,2,1, 	2,2,1);
    dummy = FfAlloc(1);
    for (i = n = 0; i < 13; ++i)
    {
	PTR v = MatGetPtr(vecs,i);
        n = MakeSeedVector(basis,n,v);
	if (n < 0)
	    Error("Out of seed vectors");
    }
    if (MakeSeedVector(basis,n,dummy) != -1)
	Error("Too many seed vectors");
    if (MatCompare(vecs,cmp) != 0)
	Error("Wrong seed vectors");
    MatFree(vecs);    
    MatFree(cmp);    
    MatFree(basis);
    SysFree(dummy);
    flags = 0;
}



