/*
**********************************************************************
*   Copyright (c) 2001-2002, International Business Machines
*   Corporation and others.  All Rights Reserved.
**********************************************************************
*   Date        Name        Description
*   11/20/2001  aliu        Creation.
**********************************************************************
*/
#ifndef UNESCTRN_H
#define UNESCTRN_H

#include "unicode/utypes.h"

#if !UCONFIG_NO_TRANSLITERATION

#include "unicode/translit.h"

U_NAMESPACE_BEGIN

/**
 * A transliterator that converts Unicode escape forms to the
 * characters they represent.  Escape forms have a prefix, a suffix, a
 * radix, and minimum and maximum digit counts.
 *
 * <p>This class is package private.  It registers several standard
 * variants with the system which are then accessed via their IDs.
 *
 * @author Alan Liu
 */
class U_I18N_API UnescapeTransliterator : public Transliterator {

 private:

    /**
     * The encoded pattern specification.  The pattern consists of
     * zero or more forms.  Each form consists of a prefix, suffix,
     * radix, minimum digit count, and maximum digit count.  These
     * values are stored as a five character header.  That is, their
     * numeric values are cast to 16-bit characters and stored in the
     * string.  Following these five characters, the prefix
     * characters, then suffix characters are stored.  Each form thus
     * takes n+5 characters, where n is the total length of the prefix
     * and suffix.  The end is marked by a header of length one
     * consisting of the character END.
     */
    UChar* spec; // owned; may not be NULL

 public:

    /**
     * Registers standard variants with the system.  Called by
     * Transliterator during initialization.
     */
    static void registerIDs();

    /**
     * Constructor.  Takes the encoded spec array (does not adopt it).
     * @param ID   the string identifier for this transliterator
     * @param spec the encoded spec array
     */
    UnescapeTransliterator(const UnicodeString& ID,
                           const UChar *spec);

    /**
     * Copy constructor.
     */
    UnescapeTransliterator(const UnescapeTransliterator&);

    /**
     * Destructor.
     */
    virtual ~UnescapeTransliterator();

    /**
     * Transliterator API.
     */
    virtual Transliterator* clone() const;

    /**
     * ICU "poor man's RTTI", returns a UClassID for the actual class.
     *
     * @draft ICU 2.2
     */
    virtual inline UClassID getDynamicClassID() const { return getStaticClassID(); }

    /**
     * ICU "poor man's RTTI", returns a UClassID for this class.
     *
     * @draft ICU 2.2
     */
    static inline UClassID getStaticClassID() { return (UClassID)&fgClassID; }

 protected:

    /**
     * Implements {@link Transliterator#handleTransliterate}.
     * @param text        the buffer holding transliterated and
     *                    untransliterated text
     * @param offset      the start and limit of the text, the position
     *                    of the cursor, and the start and limit of transliteration.
     * @param incremental if true, assume more text may be coming after
     *                    pos.contextLimit.  Otherwise, assume the text is complete.
     */
    void handleTransliterate(Replaceable& text, UTransPosition& offset,
                             UBool isIncremental) const;

 private:

    /**
     * Factory methods
     */
    static Transliterator* _createUnicode(const UnicodeString& ID, Token context);
    static Transliterator* _createJava(const UnicodeString& ID, Token context);
    static Transliterator* _createC(const UnicodeString& ID, Token context);
    static Transliterator* _createXML(const UnicodeString& ID, Token context);
    static Transliterator* _createXML10(const UnicodeString& ID, Token context);
    static Transliterator* _createPerl(const UnicodeString& ID, Token context);
    static Transliterator* _createAny(const UnicodeString& ID, Token context);

    static UChar* copySpec(const UChar* spec);

    /**
     * The address of this static class variable serves as this class's ID
     * for ICU "poor man's RTTI".
     */
    static const char fgClassID;
};

U_NAMESPACE_END

#endif /* #if !UCONFIG_NO_TRANSLITERATION */

#endif
