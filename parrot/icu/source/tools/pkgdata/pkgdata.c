/******************************************************************************
*
*   Copyright (C) 2000-2003, International Business Machines
*   Corporation and others.  All Rights Reserved.
*
*******************************************************************************
*   file name:  pkgdata.c
*   encoding:   ANSI X3.4 (1968)
*   tab size:   8 (not used)
*   indentation:4
*
*   created on: 2000may15
*   created by: Steven \u24C7 Loomis
*
*   This program packages the ICU data into different forms
*   (DLL, common data, etc.)
*/

#include <stdio.h>
#include <stdlib.h>
#include "unicode/utypes.h"
#include "unicode/putil.h"
#include "cmemory.h"
#include "cstring.h"
#include "filestrm.h"
#include "toolutil.h"
#include "unewdata.h"
#include "uoptions.h"

#if U_HAVE_POPEN
# include <unistd.h>
#endif

U_CDECL_BEGIN
#include "pkgtypes.h"
#include "makefile.h"
U_CDECL_END

static int executeMakefile(const UPKGOptions *o);
static void loadLists(UPKGOptions *o, UErrorCode *status);

/* always have this fcn, just might not do anything */
static void fillInMakefileFromICUConfig(UOption *option);

/* This sets the modes that are available */
static struct
{
    const char *name, *alt_name;
    UPKGMODE   *fcn;
    const char *desc;
} modes[] =
{
    { "files", 0, pkg_mode_files, "Uses raw data files (no effect). Installation copies all files to the target location." },
#ifdef WIN32
    { "dll",    "library", pkg_mode_windows,    "Generates one common data file and one shared library, <package>.dll"},
    { "common", "archive", pkg_mode_windows,    "Generates just the common file, <package>.dat"},
    { "static", "static",  pkg_mode_windows,    "Generates one statically linked library, " LIB_PREFIX "<package>" UDATA_LIB_SUFFIX }
#else /*#ifdef WIN32*/
#ifdef UDATA_SO_SUFFIX
    { "dll",    "library", pkg_mode_dll,    "Generates one shared library, <package>" UDATA_SO_SUFFIX },
#endif
    { "common", "archive", pkg_mode_common, "Generates one common data file, <package>.dat" },
    { "static", "static",  pkg_mode_static, "Generates one statically linked library, " LIB_PREFIX "<package>" UDATA_LIB_SUFFIX }
#endif /*#ifdef WIN32*/
};

static UOption options[]={
    /*00*/    UOPTION_DEF( "name",    'p', UOPT_REQUIRES_ARG),
    /*01*/    UOPTION_DEF( "bldopt",  'O', UOPT_REQUIRES_ARG), /* on Win32 it is release or debug */
    /*02*/    UOPTION_DEF( "mode",    'm', UOPT_REQUIRES_ARG),
    /*03*/    UOPTION_HELP_H,                                   /* -h */
    /*04*/    UOPTION_HELP_QUESTION_MARK,                       /* -? */
    /*05*/    UOPTION_VERBOSE,                                  /* -v */
    /*06*/    UOPTION_COPYRIGHT,                                /* -c */
    /*07*/    UOPTION_DEF( "comment", 'C', UOPT_REQUIRES_ARG),
    /*08*/    UOPTION_DESTDIR,                                  /* -d */
    /*09*/    UOPTION_DEF( "clean",   'k', UOPT_NO_ARG),
    /*10*/    UOPTION_DEF( "nooutput",'n', UOPT_NO_ARG),
    /*11*/    UOPTION_DEF( "rebuild", 'F', UOPT_NO_ARG),
    /*12*/    UOPTION_DEF( "tempdir", 'T', UOPT_REQUIRES_ARG),
    /*13*/    UOPTION_DEF( "install", 'I', UOPT_REQUIRES_ARG),
    /*14*/    UOPTION_SOURCEDIR ,
    /*15*/    UOPTION_DEF( "entrypoint", 'e', UOPT_REQUIRES_ARG),
    /*16*/    UOPTION_DEF( "revision", 'r', UOPT_REQUIRES_ARG),
    /*17*/    UOPTION_DEF( 0, 'M', UOPT_REQUIRES_ARG),
    /*18*/    UOPTION_DEF( "force-prefix", 'f', UOPT_NO_ARG)
};

const char options_help[][160]={
    "Set the data name",
#ifdef WIN32
    "R:icupath for release version or D:icupath for debug version, where icupath is the directory where ICU is located",
#else
    "Specify options for the builder. (Autdetected if icu-config is available)",
#endif
    "Specify the mode of building (see below; default: common)",
    "This usage text",
    "This usage text",
    "Make the output verbose",
    "Use the standard ICU copyright",
    "Use a custom comment (instead of the copyright)",
    "Specify the destination directory for files",
    "Clean out generated & temporary files",
    "Suppress output of data, just list files to be created",
    "Force rebuilding of all data",
    "Specify temporary dir (default: output dir)",
    "Install the data (specify target)",
    "Specify a custom source directory",
    "Specify a custom entrypoint name (default: short name)",
    "Specify a version when packaging in DLL or static mode",
    "Pass the next argument to make(1)",
    "Add package to all file names if not present"
};

const char  *progname = "PKGDATA";

int
main(int argc, char* argv[]) {
    FileStream  *out;
    UPKGOptions  o;
    CharList    *tail;
    UBool        needsHelp = FALSE;
    UErrorCode   status = U_ZERO_ERROR;
    char         tmp[1024];
    int32_t i;

    U_MAIN_INIT_ARGS(argc, argv);

    progname = argv[0];

    options[2].value = "common";
    options[17].value = "";

    /* read command line options */
    argc=u_parseArgs(argc, argv, sizeof(options)/sizeof(options[0]), options);

    /* error handling, printing usage message */
    /* I've decided to simply print an error and quit. This tool has too
    many options to just display them all of the time. */

    if(options[3].doesOccur || options[4].doesOccur) {
        needsHelp = TRUE;
    }
    else {
        if(!needsHelp && argc<0) {
            fprintf(stderr,
                "%s: error in command line argument \"%s\"\n",
                progname,
                argv[-argc]);
            fprintf(stderr, "Run '%s --help' for help.\n", progname);
            return 1;
        }

        if(!options[1].doesOccur) {
          /* Try to fill in from icu-config or equivalent */
          fillInMakefileFromICUConfig(&options[1]);
        }
        
        if(!options[1].doesOccur) {
          fprintf(stderr, " required parameter is missing: -O is required \n");
          fprintf(stderr, "Run '%s --help' for help.\n", progname);
          return 1;
        }

        if(!options[0].doesOccur) /* -O we already have - don't report it. */
        {
          fprintf(stderr, " required parameter -p is missing \n");
          fprintf(stderr, "Run '%s --help' for help.\n", progname);
          return 1;
        }

        if(argc == 1) {
            fprintf(stderr,
                "No input files specified.\n"
                "Run '%s --help' for help.\n", progname);
            return 1;
        }
    }   /* end !needsHelp */

    if(argc<0 || needsHelp  ) {
        fprintf(stderr,
            "usage: %s [-options] [-] [packageFile] \n"
            "\tProduce packaged ICU data from the given list(s) of files.\n"
            "\t'-' by itself means to read from stdin.\n"
            "\tpackageFile is a text file containing the list of files to package.\n",
            progname);

        fprintf(stderr, "\n options:\n");
        for(i=0;i<(sizeof(options)/sizeof(options[0]));i++) {
            fprintf(stderr, "%-5s -%c %s%-10s  %s\n",
                (i<2?"[REQ]":""),
                options[i].shortName,
                options[i].longName ? "or --" : "     ",
                options[i].longName ? options[i].longName : "",
                options_help[i]);
        }

        fprintf(stderr, "modes: (-m option)\n");
        for(i=0;i<(sizeof(modes)/sizeof(modes[0]));i++) {
            fprintf(stderr, "   %-9s ", modes[i].name);
            if (modes[i].alt_name) {
                fprintf(stderr, "/ %-9s", modes[i].alt_name);
            } else {
                fprintf(stderr, "           ");
            }
            fprintf(stderr, "  %s\n", modes[i].desc);
        }
        return 1;
    }

    /* OK, fill in the options struct */
    uprv_memset(&o, 0, sizeof(o));

    o.mode      = options[2].value;
    o.version   = options[16].doesOccur ? options[16].value : 0;
    o.makeArgs  = options[17].value;

    o.fcn = NULL;

    for(i=0;i<sizeof(modes)/sizeof(modes[0]);i++) {
        if(!uprv_strcmp(modes[i].name, o.mode)) {
            o.fcn = modes[i].fcn;
            break;
        } else if (modes[i].alt_name && !uprv_strcmp(modes[i].alt_name, o.mode)) {
            o.mode = modes[i].name;
            o.fcn = modes[i].fcn;
            break;
        }
    }

    if(o.fcn == NULL) {
        fprintf(stderr, "Error: invalid mode '%s' specified. Run '%s --help' to list valid modes.\n", o.mode, progname);
        return 1;
    }

    o.shortName = options[0].value;
    /**/ {
        int len = uprv_strlen(o.shortName);
        char *csname, *cp;
        const char *sp;

        cp = csname = (char *) uprv_malloc((len + 1 + 1) * sizeof(*o.cShortName));
        if (*(sp = o.shortName)) {
            *cp++ = isalpha(*sp) ? * sp : '_';
            for (++sp; *sp; ++sp) {
                *cp++ = isalnum(*sp) ? *sp : '_';
            }
        }
        *cp = 0;

        o.cShortName = csname;
    }

#ifdef WIN32 /* format is R:pathtoICU or D:pathtoICU */
    {
        char *pathstuff = (char *)options[1].value;
        if(options[1].value[uprv_strlen(options[1].value)-1] == '\\') {
            pathstuff[uprv_strlen(options[1].value)-1] = '\0';
        }
        if(*pathstuff == 'R' || *pathstuff == 'D') {
            o.options = pathstuff;
            pathstuff++;
            if(*pathstuff == ':') {
                *pathstuff = '\0';
                pathstuff++;
            } else {
                fprintf(stderr, "Error: invalid windows build mode, should be R (release) or D (debug).\n", o.mode, progname);
                return 1;
            }
        } else {
            fprintf(stderr, "Error: invalid windows build mode, should be R (release) or D (debug).\n", o.mode, progname);
            return 1;
        }
        o.icuroot = pathstuff;
    }
#else /* on UNIX, we'll just include the file... */
    o.options   = options[1].value;
#endif
    o.verbose   = options[5].doesOccur;
    if(options[6].doesOccur) {
        o.comment = U_COPYRIGHT_STRING;
    } else if (options[7].doesOccur) {
        o.comment = options[7].value;
    }

    if( options[8].doesOccur ) {
        o.targetDir = options[8].value;
    } else {
        o.targetDir = ".";  /* cwd */
    }

    o.clean     = options[9].doesOccur;
    o.nooutput  = options[10].doesOccur;
    o.rebuild   = options[11].doesOccur;

    if( options[12].doesOccur ) {
        o.tmpDir    = options[12].value;
    } else {
        o.tmpDir    = o.targetDir;
    }

    if( options[13].doesOccur ) {
        o.install  = options[13].value;
    }

    if( options[14].doesOccur ) {
        o.srcDir   = options[14].value;
    } else {
        o.srcDir   = ".";
    }

    if( options[15].doesOccur ) {
        o.entryName = options[15].value;
    } else {
        o.entryName = o.cShortName;
    }

    /* OK options are set up. Now the file lists. */
    tail = NULL;
    for( i=1; i<argc; i++) {
        if ( !uprv_strcmp(argv[i] , "-") ) {
            /* stdin */
            if( o.hadStdin == TRUE ) {
                fprintf(stderr, "Error: can't specify '-' twice!\n"
                    "Run '%s --help' for help.\n", progname);
                return 1;
            }
            o.hadStdin = TRUE;
        }

        o.fileListFiles = pkg_appendToList(o.fileListFiles, &tail, uprv_strdup(argv[i]));
    }

    /* load the files */
    loadLists(&o, &status);
    if( U_FAILURE(status) ) {
        fprintf(stderr, "error loading input file lists: %s\n", u_errorName(status));
        return 2;
    }

    /* Makefile pathname */
    uprv_strcpy(tmp, o.tmpDir);
    uprv_strcat(tmp, U_FILE_SEP_STRING);
    uprv_strcat(tmp, o.shortName);
    uprv_strcat(tmp, "_");
    uprv_strcat(tmp, o.mode);
    uprv_strcat(tmp, ".mak");  /* MAY NEED TO CHANGE PER PLATFORM */

    o.makeFile = uprv_strdup(tmp);

    out = T_FileStream_open(o.makeFile, "w");
    if (out) {
        pkg_mak_writeHeader(out, &o); /* need to take status */
        o.fcn(&o, out, &status);
        pkg_mak_writeFooter(out, &o);
        T_FileStream_close(out);
    } else {
        fprintf(stderr, "warning: couldn't create %s, will use existing file if any\n", o.makeFile);
        /*status = U_FILE_ACCESS_ERROR;*/
    }

    if(U_FAILURE(status)) {
        fprintf(stderr, "Error creating makefile [%s]: %s\n", o.mode,
            u_errorName(status));
        return 1;
    }

    if(o.nooutput == TRUE) {
        return 0; /* nothing to do. */
    }

    return executeMakefile(&o);
}

/* POSIX - execute makefile */
static int executeMakefile(const UPKGOptions *o)
{
    char cmd[1024];
    /*char pwd[1024];*/
    const char *make;
    int rc;

    make = getenv("MAKE");

    if(!make || !make[0]) {
        make = U_MAKE;
    }

    /*getcwd(pwd, 1024);*/
#ifdef WIN32
    sprintf(cmd, "%s %s%s -f \"%s\" %s %s %s %s",
        make,
        o->install ? "INSTALLTO=" : "",
        o->install ? o->install    : "",
        o->makeFile,
        o->clean   ? "clean"      : "",
        o->rebuild ? "rebuild"    : "",
        o->install ? "install"    : "",
        o->makeArgs);
#elif OS400
    sprintf(cmd, "CALL GNU/GMAKE PARM(%s%s%s '-f' '%s' %s %s %s %s)",
        o->install ? "'INSTALLTO=" : "",
        o->install ? o->install    : "",
        o->install ? "'"           : "",
        o->makeFile,
        o->clean   ? "'clean'"     : "",
        o->rebuild ? "'rebuild'"   : "",
        o->install ? "'install'"   : "",
        o->makeArgs);
#else
    sprintf(cmd, "%s %s%s -f %s %s %s %s %s",
        make,
        o->install ? "INSTALLTO=" : "",
        o->install ? o->install    : "",
        o->makeFile,
        o->clean   ? "clean"      : "",
        o->rebuild ? "rebuild"    : "",
        o->install ? "install"    : "",
        o->makeArgs);
#endif
    if(o->verbose) {
        puts(cmd);
    }

    rc = system(cmd);

    if(rc < 0) {
        fprintf(stderr, "# Failed, rc=%d\n", rc);
    }

    return rc < 128 ? rc : (rc >> 8);
}


static void loadLists(UPKGOptions *o, UErrorCode *status)
{
    CharList   *l, *tail = NULL, *tail2 = NULL;
    FileStream *in;
    char        line[16384];
    char       *linePtr, *lineNext;
    const uint32_t   lineMax = 16300;
    char        tmp[1024], tmp2[1024];
    char        pkgPrefix[1024];
    int32_t     pkgPrefixLen;
    const char *baseName;
    char       *s;
    int32_t     ln;
    UBool       fixPrefix;
    
    
    fixPrefix = options[18].doesOccur;
    
    strcpy(pkgPrefix, o->shortName);
    strcat(pkgPrefix, "_");
    pkgPrefixLen=uprv_strlen(pkgPrefix);
    for(l = o->fileListFiles; l; l = l->next) {
        if(o->verbose) {
            fprintf(stdout, "# Reading %s..\n", l->str);
        }
        /* TODO: stdin */
        in = T_FileStream_open(l->str, "r");
        
        if(!in) {
            fprintf(stderr, "Error opening <%s>.\n", l->str);
            *status = U_FILE_ACCESS_ERROR;
            return;
        }
        
        ln = 0;
        
        while(T_FileStream_readLine(in, line, sizeof(line))!=NULL) {
            ln++;
            if(uprv_strlen(line)>lineMax) {
                fprintf(stderr, "%s:%d - line too long (over %d chars)\n", l->str, ln, lineMax);
                exit(1);
            }
            /* remove spaces at the beginning */
            linePtr = line;
            while(isspace(*linePtr)) {
                linePtr++;
            }
            s=linePtr;
            /* remove trailing newline characters */
            while(*s!=0) {
                if(*s=='\r' || *s=='\n') {
                    *s=0;
                    break;
                }
                ++s;
            }
            if((*linePtr == 0) || (*linePtr == '#')) {
                continue; /* comment or empty line */
            }
            
            /* Now, process the line */
            lineNext = NULL;
            
            while(linePtr && *linePtr) {
                while(*linePtr == ' ') {
                    linePtr++;
                }
                /* Find the next */
                if(linePtr[0] == '"')
                {
                    lineNext = uprv_strchr(linePtr+1, '"');
                    if(lineNext == NULL) {
                        fprintf(stderr, "%s:%d - missing trailing double quote (\")\n",
                            l->str, ln);
                        exit(1);
                    } else {
                        lineNext++;
                        if(*lineNext) {
                            if(*lineNext != ' ') {
                                fprintf(stderr, "%s:%d - malformed quoted line at position %d, expected ' ' got '%c'\n",
                                    l->str, ln,  lineNext-line, (*lineNext)?*lineNext:'0');
                                exit(1);
                            }
                            *lineNext = 0;
                            lineNext++;
                        }
                    }
                } else {
                    lineNext = uprv_strchr(linePtr, ' ');
                    if(lineNext) {
                        *lineNext = 0; /* terminate at space */
                        lineNext++;
                    }
                }
                
                /* add the file */
                s = (char*)getLongPathname(linePtr);
                
                baseName = findBasename(s);
                
                if(s != baseName) {
                    /* s was something 'long' with a path */
                    if(fixPrefix && uprv_strncmp(pkgPrefix, baseName, pkgPrefixLen)) {
                        /* path don't have the prefix, add package prefix to short and longname */
                        uprv_strcpy(tmp, pkgPrefix);
                        uprv_strcpy(tmp+pkgPrefixLen, baseName);
                        
                        uprv_strncpy(tmp2, s, uprv_strlen(s)-uprv_strlen(baseName));  /* should be:   dirpath only, ending in sep */
                        tmp2[uprv_strlen(s)-uprv_strlen(baseName)]=0;
                        uprv_strcat(tmp2, pkgPrefix);
                        uprv_strcat(tmp2, baseName);
                        
                        o->files = pkg_appendToList(o->files, &tail, uprv_strdup(tmp));
                        o->filePaths = pkg_appendToList(o->filePaths, &tail2, uprv_strdup(tmp2));
                    } else {
                        /* paths already have the prefix */
                        o->files = pkg_appendToList(o->files, &tail, uprv_strdup(baseName));
                        o->filePaths = pkg_appendToList(o->filePaths, &tail2, uprv_strdup(s));
                    }
                    
                } else { /* s was just a basename, we want to prepend source dir*/
                    /* check for prefix of package */
                    uprv_strcpy(tmp, o->srcDir);
                    uprv_strcat(tmp, o->srcDir[uprv_strlen(o->srcDir)-1]==U_FILE_SEP_CHAR?"":U_FILE_SEP_STRING);
                    
                    if(fixPrefix && strncmp(pkgPrefix,s, pkgPrefixLen)) {
                        /* didn't have the prefix - add it */
                        uprv_strcat(tmp, pkgPrefix);
                        /* make up a new basename */
                        uprv_strcpy(tmp2, pkgPrefix);
                        uprv_strcat(tmp2, s);
                        o->files = pkg_appendToList(o->files, &tail, uprv_strdup(tmp2));
                    } else {
                        o->files = pkg_appendToList(o->files, &tail, uprv_strdup(baseName));
                    }
                    uprv_strcat(tmp, s);
                    o->filePaths = pkg_appendToList(o->filePaths, &tail2, uprv_strdup(tmp));
                }
                linePtr = lineNext;
            }
        }
        T_FileStream_close(in);
  }
}

/* Try calling icu-config directly to get information */
void fillInMakefileFromICUConfig(UOption *option)
{
#if U_HAVE_POPEN
  FILE *p;
  size_t n;
  static char buf[512] = "";
  static const char cmd[] = "icu-config --incfile";

  if(options[5].doesOccur)
  {
    /* informational */
    fprintf(stderr, "%s: No -O option found, trying '%s'.\n", progname, cmd);
  }

  p = popen(cmd, "r");

  if(p == NULL)
  {
    fprintf(stderr, "%s: icu-config: No icu-config found. (fix PATH or use -O option)\n", progname);
    return;
  }
  
  n = fread(buf, 1, 511, p);

  pclose(p);

  if(n<=0)
  {
    fprintf(stderr,"%s: icu-config: Could not read from icu-config. (fix PATH or use -O option)\n", progname);
    return;
  }

  if(buf[strlen(buf)-1]=='\n')
  {
    buf[strlen(buf)-1]=0;
  }
  
  if(buf[0] == 0)
  {
    fprintf(stderr, "%s: icu-config: invalid response from icu-config (fix PATH or use -O option)\n", progname);
    return;
  }

  if(options[5].doesOccur)
  {
    /* informational */
    fprintf(stderr, "%s: icu-config: using '-O %s'\n", progname, buf);
  }
  option->value = buf;
  option->doesOccur = TRUE;
#else  /* ! U_HAVE_POPEN */

  /* no popen available */
  /* Put other OS specific ways to search for the Makefile.inc type 
     information or else fail.. */

#endif
}