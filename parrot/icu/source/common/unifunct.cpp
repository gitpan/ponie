/*
**********************************************************************
* Copyright (c) 2002, International Business Machines
* Corporation and others.  All Rights Reserved.
**********************************************************************
* $Source: /cvs/public/parrot/icu/source/common/unifunct.cpp,v $ 
* $Date: 2003/09/25 19:14:50 $ 
* $Revision: 1.2 $
**********************************************************************
*/

#include "unicode/unifunct.h"

U_NAMESPACE_BEGIN

const char UnicodeFunctor::fgClassID = 0;

UnicodeMatcher* UnicodeFunctor::toMatcher() const {
    return 0;
}

UnicodeReplacer* UnicodeFunctor::toReplacer() const {
    return 0;
}

U_NAMESPACE_END

//eof
