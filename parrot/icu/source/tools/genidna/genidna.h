/*
*******************************************************************************
*
*   Copyright (C) 1999-2003, International Business Machines
*   Corporation and others.  All Rights Reserved.
*
*******************************************************************************
*   file name:  genidn.h
*   encoding:   US-ASCII
*   tab size:   8 (not used)
*   indentation:4
*
*   created on: 2003-02-06
*   created by: Ram Viswanadha
*/

#ifndef __GENIDN_H__
#define __GENIDN_H__

#include "unicode/utypes.h"
#include "unicode/uset.h"
#include "sprpimpl.h"

/* file definitions */
#define DATA_NAME "uidna"
#define DATA_TYPE "icu"

/*
 * data structure that holds the IDN properties for one or more
 * code point(s) at build time
 */

 
/* global flags */
extern UBool beVerbose, haveCopyright;

/* prototypes */

extern void
setUnicodeVersion(const char *v);

extern void
init(void);

extern void
storeMapping(uint32_t codepoint, uint32_t* mapping,int32_t length, UBool withNorm, UErrorCode* status);
extern void
storeRange(uint32_t start, uint32_t end, UBool isUnassigned,UErrorCode* status);

extern void
generateData(const char *dataDir);

extern void
cleanUpData(void);

/*
extern void
storeIDN(uint32_t code, IDN *idn);

extern void
processData(void);


*/
#endif

/*
 * Hey, Emacs, please set the following:
 *
 * Local Variables:
 * indent-tabs-mode: nil
 * End:
 *
 */

