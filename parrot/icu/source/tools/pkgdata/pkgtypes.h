/**************************************************************************
*
*   Copyright (C) 2000, International Business Machines
*   Corporation and others.  All Rights Reserved.
*
***************************************************************************
*   file name:  pkgdata.c
*   encoding:   ANSI X3.4 (1968)
*   tab size:   8 (not used)
*   indentation:4
*
*   created on: 2000may16
*   created by: Steven \u24C7 Loomis
*
*  common types for pkgdata
*/

#ifndef _PKGTYPES
#define _PKGTYPES

/* headers */
#include "unicode/utypes.h"
#include "filestrm.h"

/* linked list */
struct _CharList;

typedef struct _CharList
{
  const char       *str;
  struct _CharList *next;
} CharList;



/* 
 * write CharList 'l' into stream 's' using deliminter 'delim' (delim can be NULL). quoted: -1 remove, 0 as is, 1 add quotes
 */
const char *pkg_writeCharList(FileStream *s, CharList *l, const char *delim, int32_t quoted);

/*
 * Same, but use line breaks. quoted: -1 remove, 0 as is, 1 add quotes
 */
const char *pkg_writeCharListWrap(FileStream *s, CharList *l, const char *delim, const char *brk, int32_t quoted);


/*
 * Count items . 0 if null
 */
uint32_t pkg_countCharList(CharList *l);

/* 
 * Prepend string to CharList. Str is adopted!
 */
CharList *pkg_prependToList(CharList *l, const char *str);

/* 
 * append string to CharList. *end or even end can be null if you don't 
 * know it.[slow]
 * Str is adopted!
 */
CharList *pkg_appendToList(CharList *l, CharList** end, const char *str);

/*
 * does list contain string?  Returns: t/f
 */
UBool  pkg_listContains(CharList *l, const char *str);

/*
 * Delete list 
 */
void pkg_deleteList(CharList *l);




/*
 * Mode package function
 */
struct UPKGOptions_;
typedef   void (UPKGMODE)(struct UPKGOptions_ *, FileStream *s, UErrorCode *status);

/*
 * Static mode - write the readme file 
 * @param opt UPKGOptions
 * @param libName Name of the .lib, etc file
 * @param status ICU error code
 */
void pkg_sttc_writeReadme(struct UPKGOptions_ *opt, const char *libName, UErrorCode *status);

/* 
 * Options to be passed throughout the program
 */

typedef struct UPKGOptions_
{
  CharList   *fileListFiles; /* list of files containing files for inclusion in the package */
  CharList   *filePaths;     /* All the files, with long paths */
  CharList   *files;         /* All the files */
  CharList   *outFiles;      /* output files [full paths] */

  const char *shortName;   /* name of what we're building */
  const char *cShortName;   /* name of what we're building as a C identifier */
  const char *entryName;   /* special entrypoint name */
  const char *targetDir;
  const char *tmpDir;
  const char *srcDir;
  const char *options;     /* Options arg */
  const char *mode;        /* Mode of building */
  const char *version;     /* Library version */
  const char *makeArgs;    /* XXX Should be a CharList! */
  const char *comment;     /* comment string */
  const char *makeFile;    /* Makefile path */
  const char *install;     /* Where to install to (NULL = don't install) */
  const char *icuroot;     /* where does ICU lives */

  UBool      rebuild;
  UBool      clean;
  UBool      nooutput;
  UBool      verbose;
  UBool      hadStdin;     /* Stdin was a dependency - don't make anything depend on the file list coming in. */

  UPKGMODE  *fcn;          /* Handler function */
} UPKGOptions;


/* set up common defines for library naming */

#ifdef WIN32
# ifndef UDATA_SO_SUFFIX
#  define UDATA_SO_SUFFIX ".DLL"
# endif
# define LIB_PREFIX ""
# define OBJ_SUFFIX ".obj"
# define UDATA_LIB_SUFFIX ".LIB"

#else  /* POSIX? */
# define LIB_PREFIX "lib"
# define OBJ_SUFFIX ".o"
# define UDATA_LIB_SUFFIX ".a"
#endif 


/* defines for common file names */
#define UDATA_CMN_PREFIX ""
#define UDATA_CMN_SUFFIX ".dat"
#define UDATA_CMN_INTERMEDIATE_SUFFIX "_dat"


#endif