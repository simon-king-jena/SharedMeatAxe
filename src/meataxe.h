/* ============================= C MeatAxe ==================================
   File:        $Id: meataxe.h,v 1.3 2007-09-03 21:35:22 mringe Exp $
   Comment:     MeatAxe library external interfaces.
   --------------------------------------------------------------------------
   (C) Copyright 2004 Michael Ringe, Lehrstuhl D fuer Mathematik,
   RWTH Aachen, Germany  <mringe@math.rwth-aachen.de>
   This program is free software; see the file COPYING for details.
   ========================================================================== */

#ifndef MEATAXE_H_INCLUDED
#define MEATAXE_H_INCLUDED

#include <stdio.h>
#include <stdarg.h>

#if defined GCC
#define MTX_PRINTF_ATTRIBUTE(f,v) __attribute__((format(printf,f,v)))
#else
#define MTX_PRINTF_ATTRIBUTE(f,v)
#endif

/** @addtogroup os
  * @{
  **/

#define FM_READ	    0x01	/**< File mode: read */
#define FM_CREATE   0x02	/**< File mode: write */
#define FM_APPEND   0x03	/**< File mode: append */
#define FM_TEXT	    0x10
#define FM_LIB	    0x20
#define FM_NOERROR  0x40

/** @}
  **/

int SysCreateDirectory(const char *name);
int SysGetPid();
void SysInit(void);
void *SysMalloc(size_t nbytes);
FILE *SysFopen(const char *name, int mode);
void SysFree(void *x);
int SysFseek(FILE *f,long pos);
int SysFseekRelative(FILE *file, long distance);
void *SysRealloc(void *buf, size_t nbytes);
int SysReadLong32(FILE *f, long *buf, int n);
#define SysReadLong SysReadLong32
int SysReadLongX(FILE *f, long *buf, int n);
int SysRemoveDirectory(const char *name);
int SysRemoveFile(const char *name);
void SysSetTimeLimit(long nsecs);
long SysTimeUsed(void);
int SysWriteLong32(FILE *f, const long *buf, int n);
#define SysWriteLong SysWriteLong32
int SysWriteLongX(FILE *f, const long *buf, int n);

#define ALLOC(type) ((type *) SysMalloc(sizeof(type)))
#define NALLOC(type,n) ((type *) SysMalloc((size_t)(n) * sizeof(type)))
#define NREALLOC(x,type,n)\
	((type *) SysRealloc(x,(size_t)(n) * sizeof(type)))
#define FREE(x) SysFree(x)

/** @} **/



/* ---------------------------------------------------------------------------------------------- */

/**
 ** @addtogroup ff
 ** @{
 **/

/* Data types and constants
   ------------------------ */

#if ZZZ==0

typedef unsigned char FEL;		/**< A finite field element */
typedef FEL *PTR;			/**< A pointer to a row vector */
#define FF_ZERO ((FEL)0)		/**< The zero field element */
#define FF_ONE ((FEL)1)			/**< The unit element */
#define ZZZVERSION 6

#elif ZZZ==1

typedef unsigned short FEL;
typedef unsigned short *PTR;
#define FF_ZERO ((FEL)0xFFFF)
#define FF_ONE ((FEL)0)
#define ZZZVERSION 0x101

#else

#error "ZZZ undefined"

#endif

/* Kernel variables and functions
   ------------------------------ */

extern int FfOrder;		/**< Current field order */
extern int FfChar;		/**< Current characteristic */
extern FEL FfGen;		/**< Generator */
extern int FfNoc;		/**< Current number of columns for row ops */
extern size_t FfCurrentRowSize;
extern int FfCurrentRowSizeIo;
extern int MPB;         /** Number of marks per byte (depends on field order) */
extern int LPR;         /** Long ints per row */


/* Arithmetic */
FEL FfAdd(FEL a, FEL b);
FEL FfSub(FEL a, FEL b);
FEL FfMul(FEL a, FEL b);
FEL FfDiv(FEL a, FEL b);
FEL FfNeg(FEL a);
FEL FfInv(FEL a);

int FfMakeTables(int field);
int FfSetField(int field);
int FfSetNoc(int noc);


void FfAddMulRow(PTR dest, PTR src, FEL f);
void FfAddMulRowPartial(PTR dest, PTR src, FEL f, int first, int len);
PTR FfAddRow(PTR dest, PTR src);
PTR FfAddRowPartial(PTR dest, PTR src, int first, int len);
PTR FfSubRow(PTR dest, PTR src);
PTR FfSubRowPartial(PTR dest, PTR src, int first, int len);
PTR FfSubRowPartialReverse(PTR dest, PTR src, int first, int len);
PTR FfAlloc(int nor);
int FfCmpRows(PTR p1, PTR p2);
void FfCleanRow(PTR row, PTR matrix, int nor, const int *piv);
int FfCleanRow2(PTR row, PTR matrix, int nor, const int *piv, PTR row2);
int FfCleanRowAndRepeat(PTR row, PTR mat, int nor, const int *piv,
    PTR row2, PTR mat2);
void FfCopyRow(PTR dest, PTR src);
FEL FfEmbed(FEL a, int subfield);
FEL FfExtract(PTR row, int col);
void FfExtractColumn(PTR mat,int nor,int col,PTR result);
int FfFindPivot(PTR row, FEL *mark);
void FfFree(PTR x);
FEL FfFromInt(int l);
PTR FfGetPtr(PTR base, int row);
void FfInsert(PTR row, int col, FEL mark);
void FfMulRow(PTR row, FEL mark);
FILE *FfReadHeader(const char *name, int *fld, int *nor, int *noc);
int FfReadRows(FILE *f, PTR buf, int n);
FEL FfRestrict(FEL a, int subfield);
size_t FfRowSize(int noc);
FEL FfScalarProduct(PTR a, PTR b);
int FfSeekRow(FILE *f, int pos);
int FfStepPtr(PTR *x);
void FfSwapRows(PTR dest, PTR src);
const char *FfToGap(FEL f);
int FfToInt(FEL f);
size_t FfTrueRowSize(int noc);
FILE *FfWriteHeader(const char *name, int fld, int nor, int noc);
int FfWriteRows(FILE *f, PTR buf, int n);


/* --------------------------------------------------------------------------
   Macro versions of kernel functions
   -------------------------------------------------------------------------- */


#if ZZZ==0

extern FEL mtx_tmult[256][256];  /**< table for field element multiplication */
extern FEL mtx_tadd[256][256];   /**< table for field element addition */
extern FEL mtx_taddinv[256];     /**< table for field element negation */
extern FEL mtx_tmultinv[256];    /**< table for field element inversion */
extern FEL mtx_tffirst[256][2];  /**< table for value and index of the first non-zero mark in byte */
extern FEL mtx_textract[8][256]; /**< table for extraction of a mark from a byte */
extern FEL mtx_tnull[8][256];
extern FEL mtx_tinsert[8][256];  /**< table for insertion of a mark into a byte */
extern long mtx_embedord[4];
extern FEL mtx_embed[4][16];
extern FEL mtx_restrict[4][256];

#define FfAdd(a,b) ((FEL)mtx_tadd[(int)(unsigned char)a][(int)(unsigned char)b])
#define FfDiv(a,b) FfMul((a),FfInv(b))
#define FfInv(a) (mtx_tmultinv[(int)(unsigned char)a])
#define FfMul(a,b) ((FEL)mtx_tmult[(int)(unsigned char)a][(int)(unsigned char)b])
#define FfNeg(a) (mtx_taddinv[(int)(unsigned char)a])
#define FfSub(a,b) FfAdd((a),FfNeg(b))

#elif ZZZ==1

#define FfExtract(row,col) ((FEL)((row)[(col)-1]))
#define FfInsert(row,col,mark) ((void)((row)[(col)-1] = mark))


#endif

/** @} **/

/* ------------------------------------------------------------------
   Other low-level functions (zzz2.c)
   ------------------------------------------------------------------ */

void FfMapRow(PTR row, PTR matrix, int nor, PTR result);
void FfPermRow(PTR row, const long *perm, PTR result);
int FfSumAndIntersection(PTR wrk1, int *nor1, int *nor2, PTR wrk2, int *piv);


/* ------------------------------------------------------------------
   Library initialization and cleanup
   ------------------------------------------------------------------ */

extern int Mtx_IsInitialized;
extern int Mtx_IsX86;
extern int MtxOpt_UseOldWordGenerator;

int MtxInitLibrary();
void MtxCleanupLibrary();


/** @addtogroup str
 ** @{
 **/

/** A dynamic string.
 ** The only member, S, is a normal «char*» pointing to a NUL terminated
 ** text.
 ** Note however, that dynamic strings use their own memory management
 ** which cannot be mixed with the standard C library memory functions.
 ** Unused strings must be freed with StrFree(), and you must never use
 ** free() or realloc() on a dynamic string.
 **/

typedef struct {
    char *S;	/**< pointer to NUL terminated string */
} String;

String StrAlloc(size_t initial_capacity);
void StrFree(String *s);
void StrAppend(String *s, const char *text);
MTX_PRINTF_ATTRIBUTE(2,3)
void StrAppendF(String *s, const char *fmt, ...);
MTX_PRINTF_ATTRIBUTE(2,3)
void StrPrintF(String *s, const char *fmt, ...);

/**
 ** @}
 **/


/* ------------------------------------------------------------------
   Application framework
   ------------------------------------------------------------------ */

/**
 ** @addtogroup app
 ** @{
 **/

extern char MtxBinDir[1024];	/**< MeatAxe program directory */
extern char MtxLibDir[1024];	/**< MeatAxe library directory (for multiplication tables etc.) */

#define APP_MAX_ARGS 50

/** Application information structure.
This data structure is used to store information about the application.
It is used by the command line parser, e.g., to display the help
text.
\n Name is the program name,
\n Description is a one-line description of the program.
\n Help is a help text that is to be displayed when
the user invokes the program with '--help'.
Here is an example:
<PRE>
    MtxApplicationInfo_t AppInfo = { "sample", "MeatAxe sample",
        "\nSYNTAX\n"
        "    sample [-a] [-l <level>] <input> <ouput>\n"
        "\nOPTIONS\n"
        "    -a, --all ........ Output all data\n"
        "    -l, --level ...... Set output level (default: 42)\n"
     };
</PRE>
\sa AppAlloc
**/

typedef struct
{
    const char *Name;		/**< Program name. */
    const char *Description;	/**< One-line description of the program. */
    const char *Help;		/**< Help text. */
} MtxApplicationInfo_t;


/** Application data.
This data structure stores all internal data needed by the Application support functions,
such as command line arguments, temporary directory names, and more.
\sa AppAlloc
**/

typedef struct
{
    MtxApplicationInfo_t const *AppInfo;	/**< Program name and description. */
    int OrigArgC;				/**< Original argc from main(). */
    const char **OrigArgV;			/**< Original argv from main(). */
    int ArgC;					/**< Number of arguments. */
    const char **ArgV;				/**< Arguments. */
    int OptEnd;					/**< Used internally. */
    unsigned long IsDone[APP_MAX_ARGS];		/**< Used internally. */
    const char *OptArg;				/**< Used internally. */
    int OptInd;					/**< Used internally. */
    char TempDirName[200];			/**< Directory fr temporary files. */
} MtxApplication_t;


MtxApplication_t *AppAlloc(MtxApplicationInfo_t const *ai, int argc, const char **argv);
int AppFree(MtxApplication_t *a);
int AppGetOption(MtxApplication_t *app, const char *spec);
int AppGetCountedOption(MtxApplication_t *app, const char *spec);
const char *AppGetTextOption(MtxApplication_t *app, const char *spec,
    const char *dflt);
int AppGetIntOption(MtxApplication_t *app, const char *spec, int dflt,
    int min, int max);
int AppGetArguments(MtxApplication_t *app, int min_argc, int max_argc);
const char *AppCreateTempDir(MtxApplication_t *app);

#define MTX_COMMON_OPTIONS_SYNTAX \
    "[<Options>]"

#define MTX_COMMON_OPTIONS_DESCRIPTION \
    "    -Q ...................... Quiet, no messages\n" \
    "    -V ...................... Verbose, more messages\n" \
    "    -T <MaxTime> ............ Set CPU time limit [s]\n"

/** @} **/

/** @addtogroup err
 ** @{
**/

/* ------------------------------------------------------------------
   Messages and error handling
   ------------------------------------------------------------------ */
				/* Error message codes (%E arguments) */
#define MTX_ERR_NOMEM	    1	/**< Not enough memory */
#define MTX_ERR_GAME_OVER   2	/**< Time limit exceeded */
#define MTX_ERR_DIV0	    8	/**< Division by 0 or singular Matrix */
#define MTX_ERR_FILEFMT	    24	/**< Bad format */
#define MTX_ERR_BADARG	    31	/**< Bad argument */
#define MTX_ERR_RANGE	    33	/**< Out of range */
#define MTX_ERR_NOTECH	    34	/**< Matrix not in chelon form */
#define MTX_ERR_NOTSQUARE   35	/**< Matrix not square */
#define MTX_ERR_INCOMPAT    36	/**< Arguments are incompatible */
#define MTX_ERR_BADUSAGE    41	/**< Bad command line */
#define MTX_ERR_OPTION	    42	/**< Bad usage of option */
#define MTX_ERR_NARGS	    43	/**< Bad number of arguments */
#define MTX_ERR_NOTMATRIX   51	/**< Not a matrix */
#define MTX_ERR_NOTPERM	    53	/**< Not a permutation */


/**
 ** File information.
 ** This data structure contains information about a source file. The
 ** information is used when an error is reported via MtxError().
 **/
typedef struct
{
	const char *Name;		/**< Path to the file */
	const char *BaseName;	/**< Base name of the file */
}
MtxFileInfo_t;

/**
 ** Run-time errors.
 ** This data structure contains detailed information on an error that occurred
 ** inside the MeatAxe library.
 ** @see MtxSetErrorHandler
 **/
typedef struct
{
	const MtxFileInfo_t *FileInfo;	/**< File that is raising the error */
	int LineNo;						/**< Line in which the error is raised */
	const char *Text;				/**< Error message */
}
MtxErrorRecord_t;

/**
 ** Error handlers.
 ** @see MtxSetErrorHandler MtxErrorRecord_t
 **/
typedef void MtxErrorHandler_t(const MtxErrorRecord_t *);

#define MTX_DEFINE_FILE_INFO \
    static MtxFileInfo_t Mtx_ThisFile = { __FILE__, NULL };

int MtxError(MtxFileInfo_t *fi, int line, const char *text, ...);
MtxErrorHandler_t *MtxSetErrorHandler(MtxErrorHandler_t *h);


#define MTX_ERROR(msg) MtxError(&Mtx_ThisFile,__LINE__,msg)
#define MTX_ERROR1(msg,a1) MtxError(&Mtx_ThisFile,__LINE__,msg,a1)
#define MTX_ERROR2(msg,a1,a2) MtxError(&Mtx_ThisFile,__LINE__,msg,a1,a2)
#define MTX_ERROR3(msg,a1,a2,a3) MtxError(&Mtx_ThisFile,__LINE__,msg,a1,a2,a3)
#define MTX_ERROR4(msg,a1,a2,a3,a4) MtxError(&Mtx_ThisFile,__LINE__,\
    msg,a1,a2,a3,a4)
#define MTX_ERROR5(msg,a1,a2,a3,a4,a5) MtxError(&Mtx_ThisFile,__LINE__,\
    msg,a1,a2,a3,a4,a5)
#define MTX_ERROR7(msg,a1,a2,a3,a4,a5,a6,a7) MtxError(&Mtx_ThisFile,__LINE__,\
    msg,a1,a2,a3,a4,a5,a6,a7)


#define MTX_VERIFY(e) ((void)((e) ||\
    MtxError(&Mtx_ThisFile,__LINE__,"Assertion failed: %s",#e)))

#ifdef _DEBUG
#define DEBUG 1
#endif

#ifdef DEBUG
#define MTX_ASSERT(e) MTX_VERIFY(e)
#else
#define MTX_ASSERT(e)
#undef PARANOID
#endif



/* ------------------------------------------------------------------
   Messages
   ------------------------------------------------------------------ */

int MtxFormatMessage(char *buf, int bufsize, const char *msg, va_list al);
int MtxPrintMessage(FILE *f, const char *fmt, ...);

extern int MtxMessageLevel;
#define MSG0 (MtxMessageLevel >= 0)
#define MSG1 (MtxMessageLevel >= 1)
#define MSG2 (MtxMessageLevel >= 2)
#define MSG3 (MtxMessageLevel >= 3)
#define MSG4 (MtxMessageLevel >= 4)
#define MESSAGE(level,args)\
  (MtxMessageLevel>=(level) ? ( printf args , fflush(stdout), 1) : 0 )

/** @} **/


/* ------------------------------------------------------------------
   Miscellaneous
   ------------------------------------------------------------------ */

void MtxRandomInit(unsigned seed);
long int MtxRandom(void);
int MtxRandomInt(int max);
long gcd(long a, long b);
long lcm(long a, long b);


/* ------------------------------------------------------------------
   Structured text files (stfXXX.c)
   ------------------------------------------------------------------ */

/** @addtogroup stf
 ** @{
 **/

/**
 ** Structured text file.
 ** This structure is used for reading from , and writing to
 ** structured text files.
**/
typedef struct
{
    FILE *File;		/**< The stream we're using */
    char *LineBuf;	/**< Buffers one 'line' */
    char *GetPtr;	/**< Current input position */
    int LineBufSize;	/**< Current buffer size */
    int OutPos;		/**< Number of chars in current line (writing only) */
    int LineNo;		/**< Current line number (reading and writing) */
} StfData;

int StfClose(StfData *f);
StfData *StfOpen(const char *name, int mode);

int StfPut(StfData *f, const char *text);
int StfPutInt(StfData *f, int value);
int StfPutString(StfData *f, const char *text);
int StfPutVector(StfData *f, int size, const int *value);
int StfBeginEntry(StfData *f, const char *name);
int StfEndEntry(StfData *f);
int StfWriteValue(StfData *f, const char *name, const char *value);
int StfWriteInt(StfData *f, const char *name, int value);
int StfWriteString(StfData *f, const char *name, const char *value);
int StfWriteVector(StfData *f, const char *name, int size, const int *value);

int StfReadLine(StfData *f);
const char *StfGetName(StfData *f);
int StfGetInt(StfData *f, int *buf);
int StfGetString(StfData *f, char *buf, size_t bufsize);
int StfMatch(StfData *f, const char *pattern);
int StfGetVector(StfData *f, int *bufsize, int *buf);

/**
 ** @}
 **/

/* ------------------------------------------------------------------
   MeatAxe files
   ------------------------------------------------------------------ */

/** MeatAxe data file object.
The MtxFile_t structure represents a MeatAxe data file.
***/
typedef struct
{
    unsigned long Magic;	/**< Used internally. */
    int Field;			/**< Field order or type id. */
    int Nor;			/**< Number of rows. */
    int Noc;			/**< Number of columns. */
    FILE *File;			/**< File to read frmo/write to. */
    char *Name;			/**< File name. */
} MtxFile_t;

int MfIsValid(const MtxFile_t *file);
MtxFile_t *MfOpen(const char *name);
MtxFile_t *MfCreate(const char *name, int field, int nor, int noc);
int MfClose(MtxFile_t *file);
int MfReadLong(MtxFile_t *f, long *buf, int nrows);
int MfReadRows(MtxFile_t *f, PTR buf, int nrows);
int MfWriteLong(MtxFile_t *f, const long *buf, int count);
int MfWriteRows(MtxFile_t *f, PTR buf, int nrows);


/* ---------------------------------------------------------------------------------------------- */

typedef struct
{
    unsigned long Magic;/**< Used internally. */
    int Field;		/**< Field order. */
    int Nor;		/**< Number of rows. */
    int Noc;		/**< Number of columns. */
    PTR Data;		/**< Data, organized as array of rows. */
    size_t RowSize;	/**< Size (in bytes) of one row. */
    int *PivotTable;	/**< Pivot table (if matrix is in echelon form) . */
} Matrix_t;

Matrix_t *MatAdd(Matrix_t *dest, const Matrix_t *src);
Matrix_t *MatAddMul(Matrix_t *dest, const Matrix_t *src, FEL coeff);
Matrix_t *MatAlloc(int field, int nor, int noc);
int MatClean(Matrix_t *mat, const Matrix_t *sub);
int MatCompare(const Matrix_t *a, const Matrix_t *b);
int MatCopyRegion(Matrix_t *dest, int destrow, int destcol,
    const Matrix_t *src, int row1, int col1, int nrows, int ncols);
Matrix_t *MatCut(const Matrix_t *src, int row1, int col1, int nrows, int ncols);
Matrix_t *MatCutRows(const Matrix_t *src, int row1, int nrows);
Matrix_t *MatDup(const Matrix_t *src);
int MatEchelonize(Matrix_t *mat);
int MatFree(Matrix_t *mat);
PTR MatGetPtr(const Matrix_t *mat, int row);
Matrix_t *MatId(int fl, int nor);
Matrix_t *MatInverse(const Matrix_t *src);
int MatIsValid(const Matrix_t *m);
Matrix_t *MatLoad(const char *fn);
Matrix_t *MatMul(Matrix_t *dest, const Matrix_t *src);
Matrix_t *MatMulScalar(Matrix_t *dest, FEL coeff);
Matrix_t *MatMulStrassen(Matrix_t *dest, const Matrix_t *A, const Matrix_t *B);
void StrassenSetCutoff(size_t size);
long MatNullity(const Matrix_t *mat);
long MatNullity__(Matrix_t *mat);
Matrix_t *MatNullSpace(const Matrix_t *mat);
Matrix_t *MatNullSpace_(Matrix_t *mat, int flags);
Matrix_t *MatNullSpace__(Matrix_t *mat);
int MatOrder(const Matrix_t *mat);
int MatPivotize(Matrix_t *mat);
Matrix_t *MatPower(const Matrix_t *mat, long n);
void MatPrint(const char *name, const Matrix_t *m);
Matrix_t *MatRead(FILE *f);
int MatSave(const Matrix_t *mat, const char *fn);
FEL MatTrace(const Matrix_t *mat);
Matrix_t *MatTransposed(const Matrix_t *src);
int MatWrite(const Matrix_t *mat, FILE *f);

/* For internal use only */
void Mat_DeletePivotTable(Matrix_t *mat);



/* ------------------------------------------------------------------
   Greased matrices
   ------------------------------------------------------------------ */

/** @addtogroup grmat
 ** @{
 **/

/**
 ** Extraction table for greasing.
 ** This  structure is used internally for all greased matrix operations.
 **/

typedef struct {
    long ***tabs; /**< tables for different remainders of byte numbers mod grrows */
    int *nrvals;  /**< number of values produced by each table */
    int nrtabs;   /**< number of tables used */
} GrExtractionTable_t;

const GrExtractionTable_t *GrGetExtractionTable(int fl,int grrows);


/**
 ** A greased matrix.
 **/

typedef struct
{
    unsigned long Magic; /**< Used internally */
    int Field;			/**< Field size */
    int Nor;			/**< Number of rows */
    int Noc;			/**< Number of columns */
    int GrRows;			/**< Grease level (# of rows, 0=no grease) */
    int GrBlockSize;	/**< Vectors per block (= Field^GrRows) */
    int NumVecs;		/**< Total number of vectors in @a PrecalcData */
    PTR PrecalcData;	/**< Precalculated data */
    const GrExtractionTable_t *ExtrTab;		/**< Extraction table */
    int MPB;			/**< Number of marks per byte */
} GreasedMatrix_t;

void GrMapRow(PTR v,GreasedMatrix_t *M, PTR w);
GreasedMatrix_t *GrMatAlloc(const Matrix_t *m, int gr_rows);
int GrMatFree(GreasedMatrix_t *mat);
int GrMatIsValid(const GreasedMatrix_t *mat);

/**
 ** @}
 **/

/* ------------------------------------------------------------------
   Permutations
   ------------------------------------------------------------------ */

/** @addtogroup perm
 ** @{
 **/

/**
 ** A Permutation.
 **/
typedef struct
{
    unsigned long Magic;  /**< Used internally. */
    int Degree;		  /**< Degree of the permutation. */
    long *Data;		  /**< Images of 0,1,2,... */
} Perm_t;


Perm_t *PermAlloc(int deg);
int PermCompare(const Perm_t *a, const Perm_t *b);
Perm_t *PermDup(const Perm_t *src);
int PermFree(Perm_t *p);
Perm_t *PermInverse(const Perm_t *src);
int PermIsValid(const Perm_t *p);
Perm_t *PermLoad(const char *fn);
Perm_t *PermMul(Perm_t *dest, const Perm_t *src);
int PermOrder(const Perm_t *perm);
void PermPrint(const char *name, const Perm_t *perm);
Perm_t *PermPower(const Perm_t *p, int n);
Perm_t *PermRead(FILE *f);
int PermSave(const Perm_t *perm, const char *fn);
int PermWrite(const Perm_t *perm, FILE *f);

void Perm_ConvertOld(long *data, int len);

/** @} **/

/* ------------------------------------------------------------------
   Polynomials
   ------------------------------------------------------------------ */

/** @addtogroup poly
 ** @{
 **/

typedef struct
{
    unsigned long Magic;/**< Used internally. */
    int Field;		/**< Field order. */
    int Degree;		/**< Degree of the polynomial. */
    FEL *Data;		/**< Coefficients. Degree+1 values, starting with the
			     constant term. */
    int BufSize;	/**< Used internally for memory management. */
}
Poly_t;


Poly_t *PolAdd(Poly_t *dest, const Poly_t *src);
Poly_t *PolAlloc(int fl, int n);
int PolCompare(const Poly_t *a, const Poly_t *b);
Poly_t *PolDerive(Poly_t *pol);
Poly_t *PolDivMod(Poly_t *a, const Poly_t *b);
Poly_t *PolDup(const Poly_t *p);
int PolFree(Poly_t *p);
Poly_t *PolGcd(const Poly_t *a, const Poly_t *b);
int PolGcdEx(const Poly_t *a, const Poly_t *b, Poly_t **result);
int PolIsValid(const Poly_t *p);
Poly_t *PolMod(Poly_t *a, const Poly_t *b);
void Pol_Normalize(Poly_t *p);
Poly_t *PolLoad(const char *fn);
Poly_t *PolMul(Poly_t *dest, const Poly_t *src);
void PolPrint(char *name, const Poly_t *p);
Poly_t *PolRead(FILE *f);
int PolSave(const Poly_t *pol, const char *fn);
int PolWrite(const Poly_t *p, FILE *f);

/** @} **/

/* ------------------------------------------------------------------
   Factored polynomials
   ------------------------------------------------------------------ */

typedef struct
{
    unsigned long Magic;/**< Used internally. */
    int NFactors;	/**< Number of different irreducible factors. */
    int BufSize;	/**< Used internally for memory management. */
    Poly_t **Factor;	/**< List of irreducible factors. */
    int *Mult;		/**< Multiplicity of each factor. */
} FPoly_t;

FPoly_t *FpAlloc();
int FpFree(FPoly_t *x);
int FpIsValid(const FPoly_t *p);
FPoly_t *FpMul(FPoly_t *dest, const FPoly_t *src);
FPoly_t *FpMulP(FPoly_t *dest, const Poly_t *src, int pwr);
int FpPrint(const char *name, const FPoly_t *p);

/* ---------------------------------------------------------------------------------------------- */

typedef struct
{
    unsigned long Magic;  /**< Used internally. */
    int Size;		  /**< Number of bits. */
    int BufSize;	  /**< Used internally for memory management. */
    long Data[1];         /**< The bits. The least significant bit
			       comes first.*/
} BitString_t;		  /**< A bit string. */

BitString_t *BsAlloc(int size);
int BsAnd(BitString_t *dest, const BitString_t *src);
int BsClear(BitString_t *bs, int i);
int BsClearAll(BitString_t *bs);
int BsCompare(const BitString_t *a, const BitString_t *b);
BitString_t *BsCopy(BitString_t *dest, const BitString_t *src);
BitString_t *BsDup(const BitString_t *src);
int BsFree(BitString_t *bs);
int BsIntersectionCount(const BitString_t *a, const BitString_t *b);
int BsIsSub(const BitString_t *a, const BitString_t *b);
int BsIsValid(const BitString_t *bs);
int BsMinus(BitString_t *dest, const BitString_t *src);
int BsOr(BitString_t *dest, const BitString_t *src);
void BsPrint(const char *name, const BitString_t *bs);
BitString_t *BsRead(FILE *f);
int BsSet(BitString_t *bs, int i);
int BsTest(const BitString_t *bs, int i);
int BsWrite(BitString_t *bs, FILE *f);

#ifndef DEBUG

#define BS_BPL (sizeof(long) * 8)
#define BsSet(bs,i) ((bs)->Data[(i) / BS_BPL] |= 1L << ((i) % BS_BPL))
#define BsClear(bs,i) ((bs)->Data[(i) / BS_BPL] &= ~(1L << ((i) % BS_BPL)))
#define BsTest(bs,i) (((bs)->Data[(i) / BS_BPL] & (1L << ((i) % BS_BPL))) != 0 ? 1 : 0)

#endif


/* ------------------------------------------------------------------
   Sets
   ------------------------------------------------------------------ */

typedef struct
{
    unsigned long Magic;	/**< Used internally. */
    int Size;			/**< Number of elements. */
    int BufSize;		/**< Used internally for memory management. */
    long *Data;			/**< The elements in ascending order. */
} Set_t;


Set_t *SetAlloc();
int SetContains(const Set_t *set, long elem);
Set_t *SetDup(const Set_t *s);
int SetFree(Set_t *x);
int SetInsert(Set_t *set, long elem);
int SetIsValid(const Set_t *s);
int SetPrint(char *name, const Set_t *s);


/* ------------------------------------------------------------------
   Integer matrices
   ------------------------------------------------------------------ */

typedef struct
{
    unsigned long Magic;
    int Nor;	/**< Number of rows. */
    int Noc;	/**< Number of colums. */
    long *Data;	/**< Marks (row by row). */
} IntMatrix_t;

IntMatrix_t *ImatAlloc(int nor, int noc);
int ImatFree(IntMatrix_t *mat);
int ImatIsValid(const IntMatrix_t *m);
IntMatrix_t *ImatLoad(const char *fn);
IntMatrix_t *ImatRead(FILE *f);
int ImatSave(const IntMatrix_t *mat, const char *file_name);
int ImatWrite(const IntMatrix_t *mat, FILE *f);


/* --------------------------------------------------------------------------
   Polymorphic objects
   -------------------------------------------------------------------------- */

void *XDup(void *a);
int XIsCompatible(void *a, void *b);
void XFree(void *a);
void *XInverse(void *a);
void *XLoad(const char *fn);
void XMul(void *a, void *b);
long XOrder(void *a);
void *XPower(void *a, int n);
int XSave(void *a, const char *fn);







/* --------------------------------------------------------------------------
   Matrix sets
   -------------------------------------------------------------------------- */

/**
 ** An element of a matrix set.
 **/

typedef struct
{
    Matrix_t *Matrix;
    int PivRow;
    int PivCol;
    FEL PivMark;
} MatrixSetElement_t;


/**
 ** A set of matrices.
 **/
typedef struct {
    unsigned long Magic;
    int Len;
    MatrixSetElement_t *List;
} MatrixSet_t;

MatrixSet_t *MsAlloc();
int MsClean(const MatrixSet_t *set, Matrix_t *mat);
int MsCleanAndAppend(MatrixSet_t *set, Matrix_t *mat);
int MsFree(MatrixSet_t *set);
int MsIsValid(const MatrixSet_t *set);



/* --------------------------------------------------------------------------
   Matrix representations
   -------------------------------------------------------------------------- */

typedef struct
{
    unsigned long Magic;
    int NGen;
    Matrix_t **Gen;
} MatRep_t;

#define MR_COPY_GENERATORS  0x0001

int MrAddGenerator(MatRep_t *rep, Matrix_t *gen, int flags);
MatRep_t *MrAlloc(int ngen, Matrix_t **gen, int flags);
int MrChangeBasis(MatRep_t *rep, const Matrix_t *trans);
int MrIsValid(const MatRep_t *rep);
int MrFree(MatRep_t *rep);
MatRep_t *MrLoad(const char *basename, int ngen);
int MrSave(const MatRep_t *rep, const char *basename);
MatRep_t *MrTransposed(const MatRep_t *rep);



/* ------------------------------------------------------------------
   The word generator
   ------------------------------------------------------------------ */

typedef struct
{
    const MatRep_t *Rep;	/**< The representation. **/
    Matrix_t *Basis[8];		/**< Products of the generators **/
    int N2[8];			/**< Coefficients **/
    int *Description;		/**< Symbolic description of a word **/
} WgData_t;


WgData_t *WgAlloc(const MatRep_t *rep);
int *WgDescribeWord(WgData_t *b, long n);
int WgFree(WgData_t *b);
Matrix_t *WgMakeWord(WgData_t *b, long n);
void WgMakeFingerPrint(WgData_t *b, int fp[6]);
const char *WgSymbolicName(WgData_t *b, long n);





/* ------------------------------------------------------------------
   Spin-up, Split, Quotients, etc.
   ------------------------------------------------------------------ */

#define SF_FIRST	0x0001	/* Try only the first seed vector */
#define SF_EACH		0x0002	/* Try each seed vector */
#define SF_MAKE		0x0004	/* Try all 1-dimensional subspaces */
#define SF_SUB		0x0010	/* Try until finding a proper subspace */
#define SF_CYCLIC	0x0020	/* Try until finding a cyclic vector */
#define SF_COMBINE	0x0040	/* Combine the spans */
#define SF_SEED_MASK	0x000F
#define SF_MODE_MASK	0x00F0
#define SF_STD		0x0100	/* Spin up 'canonically' */

Matrix_t *QProjection(const Matrix_t *subspace, const Matrix_t *vectors);
Matrix_t *QAction(const Matrix_t *sub, const Matrix_t *gen);
Matrix_t *SAction(const Matrix_t *sub, const Matrix_t *gen);

typedef struct
{
    int MaxSubspaceDimension;
    int MaxTries;
    int Result;
} SpinUpInfo_t;

int SpinUpInfoInit(SpinUpInfo_t *info);
Matrix_t *SpinUp(const Matrix_t *seed, const MatRep_t *rep, int flags,
    IntMatrix_t **script, SpinUpInfo_t *info);
Matrix_t *SpinUpWithScript(const Matrix_t *seed, const MatRep_t *rep,
    const IntMatrix_t *script);
int Split(Matrix_t *subspace, const MatRep_t *rep,
	  MatRep_t **sub, MatRep_t **quot);

int ConvertSpinUpScript(IntMatrix_t *script);

Matrix_t *SpinUpWithPermutations(const Matrix_t *seed, int ngen,
    const Perm_t **gen, int flags, IntMatrix_t **script, SpinUpInfo_t *info);


/* ------------------------------------------------------------------
   Seed vector generator
   ------------------------------------------------------------------ */

long MakeSeedVector(const Matrix_t *basis, long lastno, PTR vec);



/* ------------------------------------------------------------------
   Miscellaneous algorithms
   ------------------------------------------------------------------ */

Matrix_t *MatInsert_(Matrix_t *mat, const Poly_t *pol);
Matrix_t *MatInsert(const Matrix_t *mat, const Poly_t *pol);
int IsSubspace(const Matrix_t *sub, const Matrix_t *space, int ngen);

Matrix_t *MatTensor(const Matrix_t *m1, const Matrix_t *m2);
int MatrixToVector(const Matrix_t *mat, Matrix_t *vecs, int n);
Matrix_t *VectorToMatrix(Matrix_t *vecs, int n, int noc);
Matrix_t *TensorMap(Matrix_t *vec, const Matrix_t *a, const Matrix_t *b);

int StablePower(const Matrix_t *mat, int *pwr, Matrix_t **ker);
int StablePower_(Matrix_t *mat, int *pwr, Matrix_t **ker);



/* ------------------------------------------------------------------
   Polynomial factorization (Berlekamp algorithm)
   ------------------------------------------------------------------ */

FPoly_t *Factorization(const Poly_t *pol);



/* ------------------------------------------------------------------
   Characteristic and minimal polynomials (charpol.c, minpol.c)
   ------------------------------------------------------------------ */

extern long CharPolSeed;
Poly_t *CharPolFactor(const Matrix_t *mat);
FPoly_t *CharPol(const Matrix_t *mat);
Poly_t *MinPolFactor(Matrix_t *mat);
FPoly_t *MinPol(Matrix_t *mat);



/* ------------------------------------------------------------------
   The lattice package
   ------------------------------------------------------------------ */
/**
 ** @addtogroup cfinfo
 ** @{
 **/

#define MAXGEN 20	/* Max. number of generators */
#define LAT_MAXCF 200	/* Max. number of composition factors */
#define MAXCYCL 30000	/* Max. number of cyclic submodules */
#define MAXDOTL 90000	/* Max. number of dotted lines */
#define MAXNSUB 20000	/* Max. number of submodules */
#define LAT_MAXBASENAME 100

typedef struct
{
    long dim, num, mult;
    long idword;		/**< Identifying word */
    Poly_t *idpol;
    long peakword;		/**< Peak word */
    Poly_t *peakpol;
    long nmount;		/**< Number of mountains */
    long ndotl;			/**< Number of dotted lines */
    long spl;			/**< Degree of splitting field */
}
CfInfo;

typedef struct
{
    char BaseName[LAT_MAXBASENAME];	/**< Base name */
    int Field;				/**< Field order */
    int NGen;				/**< Number of generators */
    int NCf;				/**< Number of irred. constituents */
    CfInfo Cf[LAT_MAXCF];		/**< Data for irred. constituents */
    int NSocles;			/**< Loewy length */
    int *Socle;				/**< Mult. of constituents in socles */
    int NHeads;				/**< Number of radical layers */
    int *Head;				/**< Mult. of constituents in Heads */
} Lat_Info;

int Lat_ReadInfo(Lat_Info *li, const char *basename);
int Lat_WriteInfo(const Lat_Info *li);
const char *Lat_CfName(const Lat_Info *li, int cf);
int Lat_AddHead(Lat_Info *li, int *mult);
int Lat_AddSocle(Lat_Info *li, int *mult);


#define LAT_RG_INVERT		0x0001	/* Invert generators */
#define LAT_RG_TRANSPOSE	0x0002	/* Transpose generators */
#define LAT_RG_STD		0x0004	/* Use standard form */

MatRep_t *Lat_ReadCfGens(Lat_Info *info, int cf, int flags);

/**
 ** @}
 **/


/* ------------------------------------------------------------------
   Tensor condensation package
   ------------------------------------------------------------------ */

/**
 ** @ingroup tp
 ** Tensor condensation state.
 ** The %TkData_t structure is used by the tensor condensation algorithm to store
 ** state information. By use of TK_ReadInfo() and TK_WriteInfo() a program can read
 ** the information from a file, and write back the data to a file if it was modified.
 **/

typedef struct
{
    char NameM[LAT_MAXBASENAME];	/**< Name of right factor */
    char NameN[LAT_MAXBASENAME];	/**< Name of left factor */
    int Dim;				/**< Dimension of condensed module */
    int NCf;				/**< Number of relevant constituents */
    int CfIndex[2][LAT_MAXCF];		/**< Constituent number */
} TkData_t;

int TK_ReadInfo(TkData_t *tki, const char *name);
int TK_WriteInfo(TkData_t *tki, const char *name);




/* !!!!!!!!!!!!!!! 2.3 STUFF below !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! */
/* !!!!!!!!!!!!!!! 2.3 STUFF below !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! */
/* !!!!!!!!!!!!!!! 2.3 STUFF below !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! */



/* ------------------------------------------------------------------
   Return codes
   ------------------------------------------------------------------ */

#define	EXIT_OK		0	/* Exit code: normal end */
#define EXIT_ERR	1	/*            error */



/* ------------------------------------------------------------------
   Function operating on representations and vector spaces
   ------------------------------------------------------------------ */

int IsIsomorphic(const MatRep_t *rep1, const CfInfo *info1,
    const MatRep_t *rep2, Matrix_t  **trans, int use_pw);
int MakeEndomorphisms(const MatRep_t *rep, const Matrix_t *nsp,
    Matrix_t *endo[]);
Matrix_t *HomogeneousPart(MatRep_t *m, MatRep_t *s, Matrix_t *npw,
    const IntMatrix_t *op, int dimends);



/* ------------------------------------------------------------------
   Lattice drawing functions
   ------------------------------------------------------------------ */

/**
 ** Node in a graphical lattice representation.
 **/
typedef struct
{
    double PosX, PosY;		/* Position [0..1] */
    unsigned long UserData;	/* User-defined attributes */
    int Layer;			/* Layer number */
    double Score;		/* Used in optimization */
    int ScoreCount;
} LdNode_t;

/**
 ** Graphical lattice representation.
 **/
typedef struct
{
    int NNodes;
    LdNode_t *Nodes;
    int *IsSub;		/* Incidence relation, <NNodes> * <NNodes> entries */
    int *LayerNo;	/* Layer numbers */
    int NLayers;
} LdLattice_t;

#define LD_ISSUB(l,i,k) ((l)->IsSub[(i) * (l)->NNodes + (k)])

LdLattice_t *LdAlloc(int num_nodes);
int LdFree(LdLattice_t *l);
int LdAddIncidence(LdLattice_t *lat, int sub, int sup);
int LdSetPositions(LdLattice_t *l);

int ChangeBasis(const Matrix_t *M, int ngen, const Matrix_t *gen[],
	Matrix_t *newgen[]);


#endif	/* !defined(_MEATAXE_H_) */


