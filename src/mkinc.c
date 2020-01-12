/* ============================= C MeatAxe ==================================
   File:        $Id: mkinc.c,v 1.1.1.1 2007/09/02 11:06:17 mringe Exp $
   Comment:     Calculate the incidences between mountains.
   --------------------------------------------------------------------------
   (C) Copyright 1998 Michael Ringe, Lehrstuhl D fuer Mathematik,
   RWTH Aachen, Germany  <mringe@math.rwth-aachen.de>
   This program is free software; see the file COPYING for details.
   ========================================================================== */

#include "meataxe.h"
#include <string.h>
#include <stdlib.h>



/* -------------------------------------------------------------------------
   Global data
   ------------------------------------------------------------------------- */

MTX_DEFINE_FILE_INFO
MatRep_t *Rep;              /* Generators */
Matrix_t *bild[LAT_MAXCF];      /* Image of peak word (gkond) */
int nmount = 0;             /* Number of mountains */
Matrix_t * mountlist[MAXCYCL];      /* List of mountains */
int MountDim[MAXCYCL];          /* Dim. of mountains */
Matrix_t **proj[MAXCYCL];       /* Projections of mountains */
int moffset[LAT_MAXCF];         /* Number of first mountain */
int *Class[MAXCYCL];            /* Classes of vectors */
BitString_t *subof[MAXCYCL];        /* Incidence matrix */
static Lat_Info LI;         /* Data from .cfinfo file */

static MtxApplicationInfo_t AppInfo = {
"mkinc", "Mountains and incidence matrix",
"\n"
"SYNTAX\n"
"    mkinc [<Options>] <Name>\n"
"\n"
"ARGUMENTS\n"
"    <Name> .................. Name of the representation\n"
"\n"
"OPTIONS\n"
MTX_COMMON_OPTIONS_DESCRIPTION
"    -G ...................... GAP output (implies -Q)\n"
"\n"
"FILES\n"
"    <Name>.cfinfo ........... IO Constituent info file\n"
"    <Name><Cf>.{1,2...} ..... I Generators on the constituents\n"
"    <Name><Cf>.{1,2...}k .... I Generators on the condensed modules\n"
"    <Name><Cf>.v ............ I Cyclic submodules, generated by MKCYCL\n"
"    <Name><Cf>.im ........... I Images used for condensation\n"
"    <Name><Cf>.k ............ I Uncondense matrices\n"
"    <Name>.v ................ O Mountains\n"
"    <Name>.mnt .............. O Mountain dimensions and classes of cyclic\n"
"                                submodules corresponding to the mountains\n"
"    <Name>.inc .............. O Incidence matrix\n"
};

static MtxApplication_t *App = NULL;

int opt_G = 0;      /* GAP output */


/* -----------------------------------------------------------------
   ReadFiles() - Read generators and images of peak words
   Return 1 on error, 0 on success
   ----------------------------------------------------------------- */

static int ReadFiles(const char *basename)

{
    char fn[200];
    int i;

    if (Lat_ReadInfo(&LI,basename) != 0)
    {
    MTX_ERROR1("Error reading %s.cfinfo",basename);
    return 1;
    }

    /* Load the generators
       ------------------- */
    Rep = MrLoad(LI.BaseName,LI.NGen);
    if (Rep == NULL)
    {
    MTX_ERROR("Cannot load generators");
    return 1;
    }

    /* Load the .im matrices
       --------------------- */
    for (i = 0; i < LI.NCf; ++i)
    {
        if (snprintf(fn,200,"%s%s.im",LI.BaseName,Lat_CfName(&LI,i))>=200)
        {
            MTX_ERROR("Buffer overflow");
            return 1;
        }
        bild[i] = MatLoad(fn);
        MatEchelonize(bild[i]);
    }
    return 0;
}


/* -------------------------------------------------------------------------
   Init() - Program initialization
   ------------------------------------------------------------------------- */

static int Init(int argc, const char **argv)

{
    if ((App = AppAlloc(&AppInfo,argc,argv)) == NULL)
    return -1;

    /* Parse command line
       ------------------ */
    opt_G = AppGetOption(App,"-G --gap");
    if (opt_G)
    MtxMessageLevel = -100;
    if (AppGetArguments(App,1,1) != 1)
    return -1;
    MESSAGE(0,("\n*** INCIDENCE MATRIX ***\n\n"));

    return ReadFiles(App->ArgV[0]);
}



/* -----------------------------------------------------------------
   WriteMountains() - Write mountains, dimensions and classes.

   This function writes the output files `XXX.v' and `XXX.mnt'.
   Return 1 on error, 0 on succes.
   ----------------------------------------------------------------- */

static int WriteMountains()

{
    FILE *f;
    char fn[200];
    int i;

    /* Write dimensions and classes
       ---------------------------- */
    f= SysFopen(strcat(strcpy(fn,LI.BaseName),".mnt"),FM_CREATE|FM_TEXT);
    if (f == NULL)
    {
    MTX_ERROR1("Cannot create %s: %S",fn);
    return 1;
    }
    MESSAGE(1,("Writing dimensions and classes to %s\n",fn));
    for (i = 0; i < nmount; ++i)
    {
    int *p;
    fprintf(f,"%4d %4d  %d ",i,MountDim[i],Class[i][0]);
    for (p = Class[i] + 1; *p >= 0; ++p)
        fprintf(f,"%d ",*p + 1);
    fprintf(f,"-1\n");
    }
    fclose(f);

    /* Write mountains
       --------------- */
    strcat(strcpy(fn,LI.BaseName),".v");
    MESSAGE(1,("Writing mountains to %s\n",fn));
    FfSetNoc(Rep->Gen[0]->Noc);
    if ((f = FfWriteHeader(fn,FfOrder,nmount,Rep->Gen[0]->Noc)) == NULL)
    {
        MTX_ERROR1("Cannot create file %s: %S",fn);
        return 1;
    }
    for (i = 0; i < nmount; ++i)
    {
        FfWriteRows(f,mountlist[i]->Data,1);
        MatFree(mountlist[i]);  /* We don't need them for step 2*/
    }
    fclose(f);
    return 0;
}


/* -----------------------------------------------------------------
   newmountain() - Take a vector, spin it up and check if it is a
    new mountain. If yes, add it to the mountain list and
    calculate the projection on each condensed module.
    Returns 1 if a new mountain has been found, 0 if not.
   ----------------------------------------------------------------- */

static int newmountain(Matrix_t *vec, int cf)

{
    Matrix_t *span, *backproj;
    int i;

    /* Spin up the vector and project back onto
       the condensed module where it came from.
       ----------------------------------------- */
    span = SpinUp(vec,Rep,SF_FIRST|SF_SUB,NULL,NULL);
    MESSAGE(2,("Next vector spins up to %d\n",span->Nor));
    backproj = QProjection(bild[cf],span);
    MatEchelonize(backproj);

    /* Check if it is a new mountain
       ----------------------------- */
    for (i = moffset[cf]; i < nmount; ++i)
    {
    if (backproj->Nor == proj[i][cf]->Nor)
    {
        int issub = IsSubspace(proj[i][cf],backproj,0);
        if (issub == -1)
        {
        MTX_ERROR("IsSubspace() failed");
        return -1;
        }
        if (issub != 0)
        break;
    }
    }

    /* If it is new, add it to the list and calculate
       the other projections. Otherwise just forget it.
       ------------------------------------------------ */
    if (i >= nmount)
    {
    int k;

    if (nmount >= MAXCYCL)
    {
        MTX_ERROR("TOO MANY MOUNTAINS, INCREASE MAXCYCL");
        return -1;
    }
    proj[nmount] = NALLOC(Matrix_t *,LI.NCf);

        MESSAGE(2,("New Mountain %d\n",(int)i));
    for (k = 0; k < LI.NCf; ++k)
    {
            MESSAGE(3,("Projecting on %d\n",k));
        if (k == cf)
            proj[nmount][cf] = backproj;
        else
        {
        proj[nmount][k] = QProjection(bild[k],span);
        MatEchelonize(proj[nmount][k]);
        }
    }
    mountlist[nmount] = vec;
    MountDim[nmount] = span->Nor;
    ++nmount;
    MatFree(span);
    return 1;
    }
    else
    {
    MatFree(backproj);
    MatFree(span);
    MatFree(vec);
    return 0;
    }
}


/* -----------------------------------------------------------------
   makeclass() - Find all cyclic subspaces of the projection of a
   mountain onto `its' irreducible.
   ----------------------------------------------------------------- */

static void makeclass(int mnt, int cf, Matrix_t *vectors)

{
    char *tmp = NALLOC(char,vectors->Nor+2);
    Matrix_t *vec;
    int k;
    int *p;
    size_t nvec;

    nvec = 0;
    MESSAGE(2,("Making equivalence class\n"));
    for (k = 0; k < vectors->Nor; ++k)
    {
    vec = MatCutRows(vectors,k,1);
    tmp[k] = 0;
        MESSAGE(3,(" %d",k));
    if (IsSubspace(vec,proj[mnt][cf],1))
    {
        tmp[k] = 1;
        ++nvec;
    }
    MatFree(vec);
    }
    MESSAGE(3,("\n"));

    p = Class[mnt] = NALLOC(int,nvec+2);
    *p++ = nvec;
    for (k = 0; k < vectors->Nor; ++k)
    {
    if (tmp[k])
        *p++ = k;
    }
    *p = -1;
    free(tmp);
}


/* -----------------------------------------------------------------
   FindMountains() - Make all mountains and calculate the projections of
    mountains on condensed modules.
    Return 1 on error, 0 on success
   ----------------------------------------------------------------- */

static int FindMountains()

{
    Matrix_t *vectors, *vec, *uk;
    char fn[200];
    int cf;
    int i;

    MESSAGE(0,("Step 1 (Mountains)\n"));
    nmount = 0;
    for (cf = 0; cf < LI.NCf; ++cf) /* For each irreducible */
    {
    MESSAGE(0,("  %s%s: ",LI.BaseName,Lat_CfName(&LI,cf)));

    /* Read the vectors and the uncondense matrix
       ------------------------------------------ */
    if (snprintf(fn, 200, "%s%s.v",LI.BaseName,Lat_CfName(&LI,cf))>=200)
    {
        MTX_ERROR("Buffer overflow");
        return 1;
    }
    vectors = MatLoad(fn);
    if (!vectors) return 1;
    if (snprintf(fn,200,"%s%s.k",LI.BaseName,Lat_CfName(&LI,cf))>=200)
    {
        MTX_ERROR("Buffer overflow");
        MatFree(vectors);
        return 1;
    }
    uk = MatLoad(fn);
    if (!uk)
    {
        MatFree(vectors);
        return 1;
    }

    /* Try each vector
       --------------- */
    moffset[cf] = nmount;
    for (i = 0; i < vectors->Nor; ++i)
    {
        if (i % 50 == 0)
        MESSAGE(1,("[%d] ",i));
        vec = MatCutRows(vectors,i,1);
        MatMul(vec,uk); /* Uncondense */
        if (newmountain(vec,cf))
        makeclass(nmount-1,cf,vectors);
    }
    LI.Cf[cf].nmount = nmount - moffset[cf];

    MatFree(vectors);
    MatFree(uk);
    MESSAGE(0,("%ld mountain%s\n",LI.Cf[cf].nmount,
        LI.Cf[cf].nmount != 1 ? "s" : ""));

    }
    MESSAGE(0,("Total: %d mountain%s\n",nmount,nmount != 1 ? "s" : ""));
    return 0;
}




/* -----------------------------------------------------------------
   WriteIncidenceMatrix() - Write the incidence matrix
   Return 1 on error, 0 on success.
   ----------------------------------------------------------------- */

static int WriteIncidenceMatrix()

{
    FILE *f;
    char fn[200];
    int i;
    long l;

    /* Write the incidence matrix
       -------------------------- */
    f = SysFopen(strcat(strcpy(fn,LI.BaseName),".inc"),FM_CREATE);
    if (f == NULL)
    {
        MTX_ERROR1("Cannot open %s: %S",fn);
        return 1;
    }
    MESSAGE(1,("Writing incidence matrix (%s)\n",fn));
    l = (long) nmount;
    SysWriteLong(f,&l,1);
    for (i = 0; i < nmount; ++i)
    if (BsWrite(subof[i],f)) return 1;
    fclose(f);

    /* Write the .cfinfo file
       ---------------------- */
    return Lat_WriteInfo(&LI);
}



/* -----------------------------------------------------------------
   CalculateIncidences() - Calculate all incidences
   ----------------------------------------------------------------- */

static void CalculateIncidences()

{
    int i, k;
    int cfi, cfk;   /* Comp. factor corresponding to mountain i,j */

    MESSAGE(0,("Step 2 (Incidences)\n"));

    /* Allocate memory for the incidence matrix
       ---------------------------------------- */
    for (i = 0; i < nmount; ++i)
    subof[i] = BsAlloc(nmount);

    /* Calculate the incidences
       ------------------------ */
    for (cfi = i = 0; i < nmount; ++i)
    {
    if (i == moffset[cfi])
        MESSAGE(0,("  %s%s\n",LI.BaseName,Lat_CfName(&LI,cfi)));
    for (cfk=0, k = 0; k < nmount; ++k)
    {
        int isubk, ksubi;
        ksubi = IsSubspace(proj[k][cfk],proj[i][cfk],0);
        isubk = IsSubspace(proj[i][cfi],proj[k][cfi],0);
        if (ksubi < 0 || isubk < 0)
        {
        MTX_ERROR("Subspace comparison failed");
        return;
        }
        if (ksubi)
        BsSet(subof[k],i);
        if (isubk)
        BsSet(subof[i],k);
        if (cfk < LI.NCf && k == moffset[cfk+1]-1)
        ++cfk;
        }
        if (cfi < LI.NCf && i == moffset[cfi+1]-1)
        ++cfi;
    }
}



/* -------------------------------------------------------------------------
   WriteResultGAP() - Write GAP output

   This function writes the mountain list and the incidence matrix in GAP
   format to stdout.
   ------------------------------------------------------------------------- */

static void WriteResultGAP()

{
    int i,  j;
    int *p;

    /* Write the incidence matrix
       -------------------------- */
    printf("MeatAxe.NMount := %d;\n", nmount);
    printf("MeatAxe.Incidences := [\n");
    for (i = 0; i < nmount; ++i)
    {
    printf("BlistList([" );
    for (j = 0 ; j < nmount ; j++)
    {
            printf(j < (nmount - 1) ? "%s," : "%s], [1])",
        BsTest(subof[i],j) ? "1" : "0" ) ;
    }

    printf( i < nmount-1 ? ",\n" : "] ;\n" ) ;
    }

    /* Write dimensions and classes
       ---------------------------- */
    printf("MeatAxe.Dimensions := [");
    for (i = 0; i < nmount-1 ; ++i)
    printf("%d,", MountDim[i]);
    printf("%d] ;\n", MountDim[nmount-1]);

    printf( "MeatAxe.Classes := [\n");
    for (i = 0; i < nmount; ++i)
    {
        printf("[%d",Class[i][0]);
    for (p = Class[i] + 1; *p >= 0 ; ++p)
    {
        printf(",%d", *p + 1);
    }
    printf("]");
    printf(i < nmount-1 ? ",\n" : "] ;\n");
    }
}



/* -----------------------------------------------------------------
   main()
   ----------------------------------------------------------------- */

int main(int argc, const char **argv)

{
    if (Init(argc,argv) != 0)
    {
    MTX_ERROR("Initialization failed");
    return -1;
    }
    FindMountains();
    if (WriteMountains()) return 1;
    CalculateIncidences();
    if (WriteIncidenceMatrix()) return 1;
    if (opt_G) WriteResultGAP();
    if (App != NULL) AppFree(App);
    return 0;
}


/**
@page prog_mkinc mkinc - Find Mountains

@section mkinc-syntax Command Line
<pre>
mkinc [@em Options] [-G] @em Name
</pre>

@par @em Options
  Standard options, see @ref prog_stdopts
@par -G
  Produce output in GAP format.
@par @em Name
  Name of the representation.

@section mkinc-inp Input Files
@par @em Name.cfinfo
  Constituent info file.
@par @em NameCf.1, @em NameCf.2, ...
  Generators on the irreducible constituents.
@par @em NameCf.1k, @em NameCf.2k, ...
  Generators on the corresponding condensed modules.
@par @em NameCf.v
  Cyclic submodules, generated by @ref prog_mkcycl "mkcycl".
@par @em NameCf.im
  Peak word images.
@par @em NameCf.k
  Uncondense matrices.

@section mkinc-out Output Files
@par @em Name.v
  Mountains.
@par @em Name.mnt
  Mountain dimensions and classes of cyclic submodules corresponding to the mountains.
@par @em Name.inc
  Incidence matrix.

@section mkinc-desc Description
This program runs in two steps. During the first step, all cyclic
cyclic submodules found by @ref prog_mkcycl "mkcycl" are uncondensed, giving the
local submodules, the "mountains", of the original module. Then, each
local submodule is projected back to the condensed module, and all
cyclic vectors which are contained in the image are found.
At the end of this step, there is a list of local submodules and,
for each local submodule, a list of cyclic subspaces in the
condensed module. This information is written to @em Name.mnt.

In the second step, @b mkinc computes the incidence relation between
local submodules. The result is a matrix which contains a 1 for
each incidence. This matrix is written to the file @em Name.inc.


@section mkinc-impl Implementation Details
The whole calculation of step 2 is done in the condensed modules.
This is possible because incidences between local submodules do not
change if they are condensed. Usually this saves a lot of both memory
and CPU time because one does not have to keep all mountains
simultaneously, and the condensed modules have a smaller dimension.
**/

