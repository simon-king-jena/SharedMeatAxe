/* ============================= C MeatAxe ==================================
   File:        $Id: zev.c,v 1.1.1.1 2007/09/02 11:06:17 mringe Exp $
   Comment:     Calculate eigenvalues and multiplitcities.
   --------------------------------------------------------------------------
   (C) Copyright 1998 Michael Ringe, Lehrstuhl D fuer Mathematik,
   RWTH Aachen, Germany  <mringe@math.rwth-aachen.de>
   This program is free software; see the file COPYING for details.
   ========================================================================== */


#include "meataxe.h"
#include <string.h>
#include <stdlib.h>



#define MAXDEG 200		/* Max. degree */
#define LINEWIDTH 2048		/* Line width for input file */


/* ------------------------------------------------------------------
   Global data
   ------------------------------------------------------------------ */

MTX_DEFINE_FILE_INFO

static FILE *src = NULL; 		/* Input file */
static Matrix_t *Matrix;		/* Matrix A */
static Matrix_t *W;			/* f(A) */
static Poly_t *Poly;

static const char *matname;
static char grpname[LINEWIDTH] = "";	/* Input selector (empty = ALL) */
static long deg;			/* Degree */
static char name[LINEWIDTH];		/* Value (atlas notation) */
static char thisgrp[LINEWIDTH];
static int opt_G = 0;			/* -G option (GAP output) */


static MtxApplicationInfo_t AppInfo = {
"zev", "Eigenvalues",
"SYNTAX\n"
"    zev [-GQV] <Matrix> [<Poly> [<Group>]]\n"
"\n"
"OPTIONS\n"
MTX_COMMON_OPTIONS_DESCRIPTION
"    -G, --gap ............... GAP output (implies -Q)\n"
"\n"
"ARGUMENTS\n"
"    <Matrix> ................ A square matrix\n"
"    <Poly> .................. Data file with polynomials (default: standard input)\n"
"    <Group> ................. Selects a group of polynomials (default: all)\n"
"\n"
"FILES\n"
"    <Matrix> ................ I The matrix\n"
"    <Poly> .................. I Polynomial definition file\n"
};



static MtxApplication_t *App = NULL;




/* ------------------------------------------------------------------
   init() - Initialze everything
   ------------------------------------------------------------------ */

static int Init(int argc, const char **argv)

{
    /* Parse command line
       ------------------ */
    if ((App = AppAlloc(&AppInfo,argc,argv)) == NULL)
	return -1;
    opt_G = AppGetOption(App,"-G --gap");
    if (opt_G) MtxMessageLevel = -100;
    if (AppGetArguments(App,1,3) < 0)
	return -1;
    switch (App->ArgC)
    {
	case 3:
	    strcpy(grpname,App->ArgV[2]);
	    /* NO BREAK! */
	case 2:
	    if (strcmp(App->ArgV[1],"-"))
	    {
		src = SysFopen(App->ArgV[1],FM_READ);
		if (src == NULL)
		    return -1;
	    }
	    else
		src = stdin;
	    break;
    }
    matname = App->ArgV[0];
    if ((Matrix = MatLoad(matname)) == NULL)
	return -1;
    if (Matrix->Nor != Matrix->Noc)
    {
	MTX_ERROR2("%s: %E",matname,MTX_ERR_NOTSQUARE);
	return -1;
    }
    FfSetField(Matrix->Field);
    FfSetNoc(Matrix->Noc);
    return 0;
}





/* ------------------------------------------------------------------
   Gauss() - Calculate nullity and print the result
   ------------------------------------------------------------------ */

void Gauss(void)

{
    static int first = 1;
    int nullity = MatNullity__(W);
    if (opt_G)	/* GAP output */
    {
	int mult = nullity / deg;

    	if (mult > 0)
    	{
	    if (!first)
		printf(" + ");
	    else
	    {
		printf("MeatAxe.BrauerChar := ");
		first = 0;
	    }
	    printf("%d*(%s)",mult,name);
	}
	if (nullity % deg != 0)
	    fprintf(stderr,"Non-integer multiplicity for %s\n",name);
    }
    else	/* Table output */
    {
	printf("%10s %20s %4ld %4ld",thisgrp,name,deg,nullity/deg);
	if (nullity % deg != 0)
	    printf(" (non-integer)\n");
	else
	    printf("\n");
	fflush(stdout);
    }
}


/* ------------------------------------------------------------------
   readln() - Read one input line, strip comments
   ------------------------------------------------------------------ */

int readln(char *buf)

{
    char *c;
    int flag = 0;
    while (!flag)
    {	if (feof(src) || ferror(src)) return 0;
	*buf = 0;
	fgets(buf,LINEWIDTH,src);
	if (*buf == '#') continue;	/* comment */
	for (c = buf; *c != 0 && *c != '\n'; ++c)
		if (*c != ' ' && *c != '\t')
			flag = 1;		/* */
	*c = 0;					/* remove EOL */
    }
    return 1;
}


/* ------------------------------------------------------------------
   getnextpol() - Read the next polynomial
   ------------------------------------------------------------------ */

int getnextpol(void)

{
    char line[LINEWIDTH];
    FEL coeff[MAXDEG+1];
    FEL poly[MAXDEG+1];		/* Polynomial */
    char *c;
    int i;
    FEL f;

    while (!feof(src))
    {
	if (!readln(line))
	    return 0;
	if (*line != ' ')	/* new group */
	{
	    strcpy(thisgrp,line);
	    continue;
	}
	if (grpname[0] == 0 || !strcmp(thisgrp,grpname))
	    break;
    }
    c = strtok(line," \t");
    strcpy(name,c);
    for (deg = 0; (c = strtok(NULL," \t")) != NULL; ++deg)
    {
	if (!strcmp(c,"-1"))
	    f = FfNeg(FF_ONE);
	else
	    f = FfFromInt(atoi(c));
	coeff[deg] = f;
    }
    --deg;
    for (i = 0; i <= (int) deg; ++i)
	poly[deg-i] = coeff[i];

    if (Poly != NULL)
	PolFree(Poly);
    Poly = PolAlloc(FfOrder,deg);
    for (i = 0; i <= deg; ++i)
	Poly->Data[i] = poly[i];

    return 1;
}




/* ------------------------------------------------------------------
   main()
   ------------------------------------------------------------------ */

int main(int argc, const char **argv)

{
    if (Init(argc,argv) != 0)
    {
	MTX_ERROR("Initialization failed");
	return 1;
    }
    while (getnextpol())
    {
	W = MatInsert(Matrix,Poly);
	Gauss();
    }
    if (opt_G) printf(";\n");
    AppFree(App);
    return 0;
}

/**
@page prog_zev zev - Eigenvalues

@section zev-syntax Command Line
<pre>
zev @em Options [-G] @em Matrix [@em Poly [@em Group]]
</pre>

@par @em Options
  Standard options, see @ref prog_stdopts.
@par -G
  GAP output.
@par @em Matrix
  Input matrix.
@par @em Poly
  Polynomials file name (default: standard input).
@par @em Group
  Group of polynomials to check (default: all groups).

@section zev-inp Input Files
@par @em Matrix
  Input matrix.
@par @em Poly
  Polynomials.


@section zev-desc Description
This program reads a matrix from @em Matrix and a list of polynomials
from @em Poly (or from the standard input).
For each input polynomial, it evaluates that function
of the input matrix, calculates the nullity, and puts out this nullity,
divided by the degree, along with a text from the input.

The program was specifically designed to assist in the calculation of
the Brauer characters of diagonalizable matrices, with the text giving
the complex number which is the Brauer character of the companion
matrix for that polynomial. Usually the polynomials have been prepared
in a separate data file and are fed into @b zev by giving the file name or
by redirecting its input. The preparation of the input polynomials
is generally a time-consuming task if it is done by hand, but there are
data files available for the most commonly used fields. These
files should be located in the library directory.
They are distributed with this release of the C
MeatAxe. If the user is familiar with the computer program system
GAP, he will find it easy to create his own data files.

If the nullity is not a multiple of the degree, @b zev prints a
warning message.


@subsection zev-pff Polynomial File Format
The data file contains the polynomials in text form.
Several polynomials can be comprised in a group, and the data file
can contain any number of groups of polynomials.
This allows several sets of polynomials to be kept in one data file
(for example, all polynomials for a given field), the appropriate
polynomials being selected trough the @em Group argument on the
command line.

The file is read and interpreted line by line. There are three
types of lines:
- Comment lines, beginning with a "#". These lines
  are simply ignored by @b zev, as are empty lines.
- Group headers. Each line beginning with a non-space character
  is interpreted as the beginning of a new group of polynomials.
  Such lines contain only one text field, the name of the group
  (up to 1023 characters).
- Lines beginning with a space are interpreted as polynomials.
  The format is
<pre>
[space]@em Name @em Coefficients
</pre>
  where @em Name is any text (up to 1023 characters), and
  @em Coefficients are the coefficients of the polynomial (in
  free format). Note that the first character must be an ordinary
  space charcter, a TAB is not allowed!
  The coefficients must use the names as specified by the arithmetic
  --- usually 0,1,...q-1.  The one exception is -1, which
  the program treats specially as "0-1" so that the cyclotomic
  polynomials can be used over all fields. The coefficients
  are in decreasing degree, starting with the coefficient of the
  highest power of x and continuing, ending up with the constant
  term.

Here is an example:
<pre>
# Sample input file for zev
# Some polynomials over GF(5)
#
p11b11
  1         1 4
  b11       1 4 4 1 3 4
  -1-b11    1 2 4 1 1 4
p13c13
  1         1 4
  c13       1 3 0 3 1
  c13*3     1 1 4 1 1
  c13*9     1 2 1 2 1
</pre>
This file contains 7 polynomials in two groups. The polynomial
"b11" in group "p11b11" is x<sup>5</sup>+4x<sup>4</sup>+4x<sup>3</sup>+x<sup>2</sup>+3x+4.


@subsection zev-of Output Format
There are two output formats. By default the nullities are printed in
tabular form giving group, name, degree and multiplicity (i.e., nullity
divided by degree) for each
polynomial. If the "-G" option is given, @b zev prints an algebraic
expression which can be read from GAP.
Here is an example with an 8 by 8 matrix over GF(3), polynomials being
read from "poly3":
<pre>
$ zev mat poly3 p8i2
     p8i2                    1    1    1
     p8i2                   -1    1    4
     p8i2                    0    2    2
     p8i2                   i2    2    0
     p8i2                  -i2    2    1
$ zev -G mat poly3 p8i2
MeatAxe.BrauerChar := 1*(1) + 4*(-1) + 2*(0) + 1*(-i2);
</pre>
Note that "i2" does not appear in the expression because its
coefficient is zero.

@section zev-impl Implementation Details
There must be enough memory to hold the input matrix and two
more matrices of the same size.
Lines in the polynomial input file must not be longer than 1023
characters.

It is not checked that the input file is a matrix.
TAB characters at the beginning of a line are not interpreted
as space.

*/
