//
//  file:  repattrn.cpp    
//
/*
***************************************************************************
*   Copyright (C) 2002-2003 International Business Machines Corporation   *
*   and others. All rights reserved.                                      *
***************************************************************************
*/

#include "unicode/utypes.h"

#if !UCONFIG_NO_REGULAR_EXPRESSIONS

#include "unicode/regex.h"
#include "uassert.h"
#include "uvector.h"
#include "uvectr32.h"
#include "regexcmp.h"
#include "regeximp.h"
#include "regexst.h"

U_NAMESPACE_BEGIN

//--------------------------------------------------------------------------
//
//    RegexPattern    Default Constructor
//
//--------------------------------------------------------------------------
RegexPattern::RegexPattern() {
    // Init all of this instances data.
    init();

    // Lazy init of all shared global sets.
    RegexStaticSets::initGlobals(&fDeferredStatus);
};


//--------------------------------------------------------------------------
//
//   Copy Constructor        Note:  This is a rather inefficient implementation,
//                                  but it probably doesn't matter.
//
//--------------------------------------------------------------------------
RegexPattern::RegexPattern(const RegexPattern &other) :  UObject(other) {
    init(); 
    *this = other;
}



//--------------------------------------------------------------------------
//
//    Assignmenet Operator
//
//--------------------------------------------------------------------------
RegexPattern &RegexPattern::operator = (const RegexPattern &other) {
    if (this == &other) {
        // Source and destination are the same.  Don't do anything.
        return *this;
    }

    // Clean out any previous contents of object being assigned to.
    zap();

    // Give target object a default initialization
    init();

    // Copy simple fields
    fPattern          = other.fPattern;
    fFlags            = other.fFlags;
    fLiteralText      = other.fLiteralText;
    fDeferredStatus   = other.fDeferredStatus;
    fMinMatchLen      = other.fMinMatchLen;
    fMaxCaptureDigits = other.fMaxCaptureDigits;
    fStaticSets       = other.fStaticSets; 
    
    fStartType        = other.fStartType;
    fInitialStringIdx = other.fInitialStringIdx;
    fInitialStringLen = other.fInitialStringLen;
    *fInitialChars    = *other.fInitialChars;
    *fInitialChars8   = *other.fInitialChars8;
    fInitialChar      = other.fInitialChar;

    //  Copy the pattern.  It's just values, nothing deep to copy.
    fCompiledPat->assign(*other.fCompiledPat, fDeferredStatus);
    fGroupMap->assign(*other.fGroupMap, fDeferredStatus);

    //  Copy the Unicode Sets.  
    //    Could be made more efficient if the sets were reference counted and shared,
    //    but I doubt that pattern copying will be particularly common. 
    //    Note:  init() already added an empty element zero to fSets
    int32_t i;
    int32_t  numSets = other.fSets->size();
    fSets8 = new Regex8BitSet[numSets];
    for (i=1; i<numSets; i++) {
        if (U_FAILURE(fDeferredStatus)) {
            return *this;
        }
        UnicodeSet *sourceSet = (UnicodeSet *)other.fSets->elementAt(i);
        UnicodeSet *newSet    = new UnicodeSet(*sourceSet);
        if (newSet == NULL) {
            fDeferredStatus = U_MEMORY_ALLOCATION_ERROR;
            break;
        }
        fSets->addElement(newSet, fDeferredStatus);
        fSets8[i] = other.fSets8[i];
    }

    return *this;
}


//--------------------------------------------------------------------------
//
//    init        Shared initialization for use by constructors.
//                Bring an uninitialized RegexPattern up to a default state.
//
//--------------------------------------------------------------------------
void RegexPattern::init() {
    fFlags            = 0;
    fDeferredStatus   = U_ZERO_ERROR;
    fMinMatchLen      = 0;
    fMaxCaptureDigits = 1;  
    fStaticSets       = NULL;
    fFrameSize        = 0;
    fDataSize         = 0;
    fStartType        = START_NO_INFO;
    fInitialStringIdx = 0;
    fInitialStringLen = 0;
    fInitialChars     = NULL;
    fInitialChars8    = NULL;
    fInitialChar      = 0;
    fSets8            = NULL;
    
    fCompiledPat      = new UVector32(fDeferredStatus);
    fGroupMap         = new UVector32(fDeferredStatus);
    fSets             = new UVector(fDeferredStatus);
    fInitialChars     = new UnicodeSet;
    fInitialChars8    = new Regex8BitSet;
    if (U_FAILURE(fDeferredStatus)) {
        return;
    }
    if (fCompiledPat == NULL  || fGroupMap == NULL || fSets == NULL ||
        fInitialChars == NULL || fInitialChars8 == NULL) {
        fDeferredStatus = U_MEMORY_ALLOCATION_ERROR;
        return;
    }

    // Slot zero of the vector of sets is reserved.  Fill it here.
    fSets->addElement((int32_t)0, fDeferredStatus);
}


//--------------------------------------------------------------------------
//
//   zap            Delete everything owned by this RegexPattern. 
//
//--------------------------------------------------------------------------
void RegexPattern::zap() {
    delete fCompiledPat;
    fCompiledPat = NULL;
    int i;
    for (i=1; i<fSets->size(); i++) {
        UnicodeSet *s;
        s = (UnicodeSet *)fSets->elementAt(i);
        if (s != NULL) {
            delete s;
        }
    }
    delete fSets;
    fSets = NULL;
    delete fGroupMap;
    fGroupMap = NULL;
    delete fInitialChars;
    fInitialChars = NULL;
    delete fInitialChars8;
    fInitialChars8 = NULL;
    delete[] fSets8;
    fSets8 = NULL;
}


//--------------------------------------------------------------------------
//
//   Destructor
//
//--------------------------------------------------------------------------
RegexPattern::~RegexPattern() {
    zap();
};


//--------------------------------------------------------------------------
//
//   Clone
//
//--------------------------------------------------------------------------
RegexPattern  *RegexPattern::clone() const { 
    RegexPattern  *copy = new RegexPattern(*this);
    return copy;
};


//--------------------------------------------------------------------------
//
//   operator ==   (comparison)    Consider to patterns to be == if the
//                                 pattern strings and the flags are the same.
//
//--------------------------------------------------------------------------
UBool   RegexPattern::operator ==(const RegexPattern &other) const {
    UBool r = this->fFlags    == other.fFlags &&
              this->fPattern  == other.fPattern &&
              this->fDeferredStatus == other.fDeferredStatus;
    return r;
}

//---------------------------------------------------------------------
//
//   compile        
//
//---------------------------------------------------------------------
RegexPattern  *RegexPattern::compile(
                             const UnicodeString &regex,
                             uint32_t             flags,
                             UParseError          &pe,
                             UErrorCode           &status)  {

    if (U_FAILURE(status)) {
        return NULL;
    }

    const uint32_t allFlags = UREGEX_CANON_EQ | UREGEX_CASE_INSENSITIVE | UREGEX_COMMENTS |
                              UREGEX_DOTALL   | UREGEX_MULTILINE;

    if ((flags & ~allFlags) != 0) {
        status = U_REGEX_INVALID_FLAG;
        return NULL;
    }

    if ((flags & UREGEX_CANON_EQ) != 0) {
        status = U_REGEX_UNIMPLEMENTED;
        return NULL;
    }

    RegexPattern *This = new RegexPattern;
    if (This == NULL) {
        status = U_MEMORY_ALLOCATION_ERROR;
        return NULL;
    }
    if (U_FAILURE(This->fDeferredStatus)) {
        status = This->fDeferredStatus;
        return NULL;
    }
    This->fFlags = flags;

    RegexCompile     compiler(This, status);
    compiler.compile(regex, pe, status);

    return This;
};
    
//
//   compile with default flags.
//
RegexPattern *RegexPattern::compile( const UnicodeString &regex,
        UParseError          &pe,
        UErrorCode           &err) 
{
    return compile(regex, 0, pe, err); 
}



//
//   compile with no UParseErr parameter.
//
RegexPattern *RegexPattern::compile( const UnicodeString &regex,
        uint32_t             flags,
        UErrorCode           &err) 
{
    UParseError pe;
    return compile(regex, flags, pe, err); 
}



//---------------------------------------------------------------------
//
//   flags
//
//---------------------------------------------------------------------
uint32_t RegexPattern::flags() const {
    return fFlags;
}


//---------------------------------------------------------------------
//
//   matcher(UnicodeString, err)
//
//---------------------------------------------------------------------
RegexMatcher *RegexPattern::matcher(const UnicodeString &input,
                                    UErrorCode          &status)  const {
    RegexMatcher    *retMatcher = matcher(status);
    if (retMatcher != NULL) {
        retMatcher->reset(input);
    }
    return retMatcher;
};



//---------------------------------------------------------------------
//
//   matcher(status)
//
//---------------------------------------------------------------------
RegexMatcher *RegexPattern::matcher(UErrorCode &status)  const {
    RegexMatcher    *retMatcher = NULL;

    if (U_FAILURE(status)) {
        return NULL;
    }
    if (U_FAILURE(fDeferredStatus)) {
        status = fDeferredStatus;
        return NULL;
    }

    retMatcher = new RegexMatcher(this); 
    if (retMatcher == NULL) {
        status = U_MEMORY_ALLOCATION_ERROR;
        return NULL;
    }
    return retMatcher;
};



//---------------------------------------------------------------------
//
//   matches        Convenience function to test for a match, starting
//                  with a pattern string and a data string.
//
//---------------------------------------------------------------------
UBool RegexPattern::matches(const UnicodeString   &regex,
              const UnicodeString   &input,
                    UParseError     &pe,
                    UErrorCode      &status) {

    if (U_FAILURE(status)) {return FALSE;}

    UBool         retVal;
    RegexPattern *pat     = NULL;
    RegexMatcher *matcher = NULL;

    pat     = RegexPattern::compile(regex, 0, pe, status);
    matcher = pat->matcher(input, status);
    retVal  = matcher->matches(status);

    delete matcher;
    delete pat;
    return retVal;
}




//---------------------------------------------------------------------
//
//   pattern
//
//---------------------------------------------------------------------
UnicodeString RegexPattern::pattern() const {
    return fPattern;
}




//---------------------------------------------------------------------
//
//   split
//
//---------------------------------------------------------------------
int32_t  RegexPattern::split(const UnicodeString &input,
        UnicodeString    dest[],
        int32_t          destCapacity,
        UErrorCode       &status) const
{
    if (U_FAILURE(status)) {
        return 0;
    };

    RegexMatcher  m(this);
    int32_t r = m.split(input, dest, destCapacity, status);
    return r;
}



//---------------------------------------------------------------------
//
//   dump    Output the compiled form of the pattern.
//           Debugging function only.
//
//---------------------------------------------------------------------
void   RegexPattern::dumpOp(int32_t index) const {
#if defined(REGEX_DEBUG)
    static const char * const opNames[] = {URX_OPCODE_NAMES};
    int32_t op          = fCompiledPat->elementAti(index);
    int32_t val         = URX_VAL(op);
    int32_t type        = URX_TYPE(op);
    int32_t pinnedType  = type;
    if (pinnedType >= sizeof(opNames)/sizeof(char *)) {
        pinnedType = 0;
    }
    
    REGEX_DUMP_DEBUG_PRINTF("%4d   %08x    %-15s  ", index, op, opNames[pinnedType]);
    switch (type) {
    case URX_NOP:
    case URX_DOTANY:
    case URX_DOTANY_ALL:
    case URX_DOTANY_PL:
    case URX_DOTANY_ALL_PL:
    case URX_FAIL:
    case URX_CARET:
    case URX_DOLLAR:
    case URX_BACKSLASH_G:
    case URX_BACKSLASH_X:
    case URX_END:
    case URX_DOLLAR_M:
    case URX_CARET_M:
        // Types with no operand field of interest.
        break;
        
    case URX_RESERVED_OP:
    case URX_START_CAPTURE:
    case URX_END_CAPTURE:
    case URX_STATE_SAVE:
    case URX_JMP:
    case URX_JMP_SAV:
    case URX_JMP_SAV_X:
    case URX_BACKSLASH_B:
    case URX_BACKSLASH_D:
    case URX_BACKSLASH_Z:
    case URX_STRING_LEN:
    case URX_CTR_INIT:
    case URX_CTR_INIT_NG:
    case URX_CTR_LOOP:
    case URX_CTR_LOOP_NG:
    case URX_RELOC_OPRND:
    case URX_STO_SP:
    case URX_LD_SP:
    case URX_BACKREF:
    case URX_STO_INP_LOC:
    case URX_JMPX:
    case URX_LA_START:
    case URX_LA_END:
    case URX_BACKREF_I:
    case URX_LB_START:
    case URX_LB_CONT:
    case URX_LB_END:
    case URX_LBN_CONT:
    case URX_LBN_END:
    case URX_LOOP_C:
    case URX_LOOP_DOT_I:
        // types with an integer operand field.
        REGEX_DUMP_DEBUG_PRINTF("%d", val);
        break;
        
    case URX_ONECHAR:
    case URX_ONECHAR_I:
        REGEX_DUMP_DEBUG_PRINTF("%c", val<256?val:'?');
        break;
        
    case URX_STRING:
    case URX_STRING_I:
        {
            int32_t lengthOp       = fCompiledPat->elementAti(index+1);
            U_ASSERT(URX_TYPE(lengthOp) == URX_STRING_LEN);
            int32_t length = URX_VAL(lengthOp);
            int32_t i;
            for (i=val; i<val+length; i++) {
                UChar c = fLiteralText[i];
                if (c < 32 || c >= 256) {c = '.';}
                REGEX_DUMP_DEBUG_PRINTF("%c", c);
            }
        }
        break;

    case URX_SETREF:
    case URX_LOOP_SR_I:
        {
            UnicodeString s;
            UnicodeSet *set = (UnicodeSet *)fSets->elementAt(val);
            set->toPattern(s, TRUE);
            for (int32_t i=0; i<s.length(); i++) {
                REGEX_DUMP_DEBUG_PRINTF("%c", s.charAt(i));
            }
        }
        break;

    case URX_STATIC_SETREF:
    case URX_STAT_SETREF_N:
        {
            UnicodeString s;
            if (val & URX_NEG_SET) {
                REGEX_DUMP_DEBUG_PRINTF("NOT ");
                val &= ~URX_NEG_SET;
            }
            UnicodeSet *set = fStaticSets[val];
            set->toPattern(s, TRUE);
            for (int32_t i=0; i<s.length(); i++) {
                REGEX_DUMP_DEBUG_PRINTF("%c", s.charAt(i));
            }
        }
        break;

        
    default:
        REGEX_DUMP_DEBUG_PRINTF("??????");
        break;
    }
    REGEX_DUMP_DEBUG_PRINTF("\n");
#endif
}



void   RegexPattern::dump() const {
#if defined(REGEX_DEBUG)
    int      index;
    int      i;

    REGEX_DUMP_DEBUG_PRINTF("Original Pattern:  ");
    for (i=0; i<fPattern.length(); i++) {
        REGEX_DUMP_DEBUG_PRINTF("%c", fPattern.charAt(i));
    }
    REGEX_DUMP_DEBUG_PRINTF("\n");
    REGEX_DUMP_DEBUG_PRINTF("   Min Match Length:  %d\n", fMinMatchLen);
    REGEX_DUMP_DEBUG_PRINTF("   Match Start Type:  %s\n", START_OF_MATCH_STR(fStartType));   
    if (fStartType == START_STRING) {
        REGEX_DUMP_DEBUG_PRINTF("    Initial match sting: \"");
        for (i=fInitialStringIdx; i<fInitialStringIdx+fInitialStringLen; i++) {
            REGEX_DUMP_DEBUG_PRINTF("%c", fLiteralText[i]);   // TODO:  non-printables, surrogates.
        }

    } else if (fStartType == START_SET) {
        int32_t numSetChars = fInitialChars->size();
        if (numSetChars > 20) {
            numSetChars = 20;
        }
        REGEX_DUMP_DEBUG_PRINTF("     Match First Chars : ");
        for (i=0; i<numSetChars; i++) {
            UChar32 c = fInitialChars->charAt(i);
            if (0x20<c && c <0x7e) { 
                REGEX_DUMP_DEBUG_PRINTF("%c ", c);
            } else {
                REGEX_DUMP_DEBUG_PRINTF("%#x ", c);
            }
        }
        if (numSetChars < fInitialChars->size()) {
            REGEX_DUMP_DEBUG_PRINTF(" ...");
        }
        REGEX_DUMP_DEBUG_PRINTF("\n");

    } else if (fStartType == START_CHAR) {
        REGEX_DUMP_DEBUG_PRINTF("    First char of Match : ");
        if (0x20 < fInitialChar && fInitialChar<0x7e) {
                REGEX_DUMP_DEBUG_PRINTF("%c\n", fInitialChar);
            } else {
                REGEX_DUMP_DEBUG_PRINTF("%#x\n", fInitialChar);
            }
    }

    REGEX_DUMP_DEBUG_PRINTF("\nIndex   Binary     Type             Operand\n"
           "-------------------------------------------\n");
    for (index = 0; index<fCompiledPat->size(); index++) {
        dumpOp(index);
    }
    REGEX_DUMP_DEBUG_PRINTF("\n\n");
#endif
};



const char RegexPattern::fgClassID = 0;

//----------------------------------------------------------------------------------
//
//   regex_cleanup      Memory cleanup function, free/delete all
//                      cached memory.  Called by ICU's u_cleanup() function.
//
//----------------------------------------------------------------------------------
U_CFUNC UBool 
regex_cleanup(void) {
    RegexCompile::cleanup();
    return TRUE;
};

U_NAMESPACE_END
#endif  // !UCONFIG_NO_REGULAR_EXPRESSIONS