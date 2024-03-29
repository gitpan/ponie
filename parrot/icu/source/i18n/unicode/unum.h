/*
*******************************************************************************
* Copyright (C) 1997-2003, International Business Machines Corporation and others. All Rights Reserved.
* Modification History:
*
*   Date        Name        Description
*   06/24/99    helena      Integrated Alan's NF enhancements and Java2 bug fixes
*******************************************************************************
*/

#ifndef _UNUM
#define _UNUM

#include "unicode/utypes.h"

#if !UCONFIG_NO_FORMATTING

#include "unicode/umisc.h"
#include "unicode/parseerr.h"
/**
 * \file
 * \brief C API: NumberFormat
 *
 * <h2> Number Format C API </h2>
 *
 * Number Format C API  Provides functions for
 * formatting and parsing a number.  Also provides methods for
 * determining which locales have number formats, and what their names
 * are.
 * <P>
 * UNumberFormat helps you to format and parse numbers for any locale.
 * Your code can be completely independent of the locale conventions
 * for decimal points, thousands-separators, or even the particular
 * decimal digits used, or whether the number format is even decimal.
 * There are different number format styles like decimal, currency,
 * percent and spellout.
 * <P>
 * To format a number for the current Locale, use one of the static
 * factory methods:
 * <pre>
 * \code
 *    UChar myString[20];
 *    double myNumber = 7.0;
 *    UErrorCode status = U_ZERO_ERROR;
 *    UNumberFormat* nf = unum_open(UNUM_DEFAULT, NULL, -1, NULL, NULL, &status);
 *    unum_formatDouble(nf, myNumber, myString, 20, NULL, &status);
 *    printf(" Example 1: %s\n", austrdup(myString) ); //austrdup( a function used to convert UChar* to char*)
 * \endcode
 * </pre>
 * If you are formatting multiple numbers, it is more efficient to get
 * the format and use it multiple times so that the system doesn't
 * have to fetch the information about the local language and country
 * conventions multiple times.
 * <pre>
 * \code
 * uint32_t i, resultlength, reslenneeded;
 * UErrorCode status = U_ZERO_ERROR;
 * UFieldPosition pos;
 * uint32_t a[] = { 123, 3333, -1234567 };
 * const uint32_t a_len = sizeof(a) / sizeof(a[0]);
 * UNumberFormat* nf;
 * UChar* result = NULL;
 *
 * nf = unum_open(UNUM_DEFAULT, NULL, -1, NULL, NULL, &status);
 * for (i = 0; i < a_len; i++) {
 *    resultlength=0;
 *    reslenneeded=unum_format(nf, a[i], NULL, resultlength, &pos, &status);
 *    result = NULL;
 *    if(status==U_BUFFER_OVERFLOW_ERROR){
 *       status=U_ZERO_ERROR;
 *       resultlength=reslenneeded+1;
 *       result=(UChar*)malloc(sizeof(UChar) * resultlength);
 *       unum_format(nf, a[i], result, resultlength, &pos, &status);
 *    }
 *    printf( " Example 2: %s\n", austrdup(result));
 *    free(result);
 * }
 * \endcode
 * </pre>
 * To format a number for a different Locale, specify it in the
 * call to unum_open().
 * <pre>
 * \code
 *     UNumberFormat* nf = unum_open(UNUM_DEFAULT, NULL, -1, "fr_FR", NULL, &success)
 * \endcode
 * </pre>
 * You can use a NumberFormat API unum_parse() to parse.
 * <pre>
 * \code
 *    UErrorCode status = U_ZERO_ERROR;
 *    int32_t pos=0;
 *    int32_t num;
 *    num = unum_parse(nf, str, u_strlen(str), &pos, &status);
 * \endcode
 * </pre>
 * Use UCAL_DECIMAL to get the normal number format for that country.
 * There are other static options available.  Use UCAL_CURRENCY
 * to get the currency number format for that country.  Use UCAL_PERCENT
 * to get a format for displaying percentages. With this format, a
 * fraction from 0.53 is displayed as 53%.
 * <P>
 * You can also control the display of numbers with such function as
 * unum_getAttribues() and unum_setAtributes().  where in you can set the
 * miminum fraction digits, grouping used etc.
 * @see UNumberFormatAttributes for more details
 * <P>
 * You can also use forms of the parse and format methods with
 * ParsePosition and UFieldPosition to allow you to:
 * <ul type=round>
 *   <li>(a) progressively parse through pieces of a string.
 *   <li>(b) align the decimal point and other areas.
 * </ul>
 * <p>
 * It is also possible to change or set the symbols used for a particular
 * locale like the currency symbol, the grouping seperator , monetary seperator
 * etc by making use of functions unum_setSymbols() and unum_getSymbols().
 */

/** A number formatter.
 *  For usage in C programs.
 *  @stable ICU 2.0
 */
typedef void* UNumberFormat;

/** The possible number format styles. 
 *  @stable ICU 2.0
 */
typedef enum UNumberFormatStyle {
    /** Ignore style specification and open the pattern */
    UNUM_IGNORE=0,
    /** Decimal format */
    UNUM_DECIMAL=1,
    /** Currency format */
    UNUM_CURRENCY,
    /** Percent format */
    UNUM_PERCENT,
    /** Scientific format */
    UNUM_SCIENTIFIC,
    /** Spellout format */
    UNUM_SPELLOUT,
    /** Default format */
    UNUM_DEFAULT = UNUM_DECIMAL
} UNumberFormatStyle;

/** The possible number format rounding modes. 
 *  @stable ICU 2.0
 */
typedef enum UNumberFormatRoundingMode {
    UNUM_ROUND_CEILING,
    UNUM_ROUND_FLOOR,
    UNUM_ROUND_DOWN,
    UNUM_ROUND_UP,
    UNUM_FOUND_HALFEVEN,
    UNUM_ROUND_HALFDOWN,
    UNUM_ROUND_HALFUP
} UNumberFormatRoundingMode;

/** The possible number format pad positions. 
 *  @stable ICU 2.0
 */
typedef enum UNumberFormatPadPosition {
    UNUM_PAD_BEFORE_PREFIX,
    UNUM_PAD_AFTER_PREFIX,
    UNUM_PAD_BEFORE_SUFFIX,
    UNUM_PAD_AFTER_SUFFIX
} UNumberFormatPadPosition;

/**
* Open a new UNumberFormat for formatting and parsing numbers.
* A UNumberFormat may be used to format numbers in calls to \Ref{unum_format},
* and to parse numbers in calls to \Ref{unum_parse}.
* @param style The type of number format to open: one of UNUM_DECIMAL, UNUM_CURRENCY,
* UNUM_PERCENT, UNUM_SPELLOUT, UNUM_DEFAULT or UNUM_IGNORE. If UNUM_IGNORE is passed
* then the style specification is ignored and a number format is opened with the pattern.
* @param pattern A pattern specifying the format to use.
* @param patternLength The number of characters in the pattern, or -1 if null-terminated.
* @param locale The locale specifying the formatting conventions
* @param parseErr a pointer to a UParseError struct to receive the
* details of any parsing errors. This parameter may be NULL if no
* parsing error details are desired.
* @param status A pointer to an UErrorCode to receive any errors
* @return A pointer to a UNumberFormat to use for formatting numbers, or 0 if
* an error occurred.
* @see unum_open
* @stable ICU 2.0
*/
U_CAPI UNumberFormat* U_EXPORT2 
unum_open(  UNumberFormatStyle    style,
            const    UChar*    pattern,
            int32_t            patternLength,
            const    char*     locale,
            UParseError*       parseErr,
            UErrorCode*        status);


/**
* Close a UNumberFormat.
* Once closed, a UNumberFormat may no longer be used.
* @param fmt The formatter to close.
* @stable ICU 2.0
*/
U_CAPI void U_EXPORT2 
unum_close(UNumberFormat* fmt);

/**
 * Open a copy of a UNumberFormat.
 * This function performs a deep copy.
 * @param fmt The format to copy
 * @param status A pointer to an UErrorCode to receive any errors.
 * @return A pointer to a UNumberFormat identical to fmt.
 * @stable ICU 2.0
 */
U_CAPI UNumberFormat* U_EXPORT2 
unum_clone(const UNumberFormat *fmt,
       UErrorCode *status);

/**
* Format an integer using a UNumberFormat.
* The integer will be formatted according to the UNumberFormat's locale.
* @param fmt The formatter to use.
* @param number The number to format.
* @param result A pointer to a buffer to receive the formatted number.
* @param resultLength The maximum size of result.
* @param pos    A pointer to a UFieldPosition.  On input, position->field
* is read.  On output, position->beginIndex and position->endIndex indicate
* the beginning and ending indices of field number position->field, if such
* a field exists.  This parameter may be NULL, in which case no field
* @param status A pointer to an UErrorCode to receive any errors
* @return The total buffer size needed; if greater than resultLength, the output was truncated.
* @see unum_formatDouble
* @see unum_parse
* @see unum_parseDouble
* @see UFieldPosition
* @stable ICU 2.0
*/
U_CAPI int32_t U_EXPORT2 
unum_format(    const    UNumberFormat*    fmt,
        int32_t            number,
        UChar*            result,
        int32_t            resultLength,
        UFieldPosition    *pos,
        UErrorCode*        status);

/**
* Format a double using a UNumberFormat.
* The double will be formatted according to the UNumberFormat's locale.
* @param fmt The formatter to use.
* @param number The number to format.
* @param result A pointer to a buffer to receive the formatted number.
* @param resultLength The maximum size of result.
* @param pos    A pointer to a UFieldPosition.  On input, position->field
* is read.  On output, position->beginIndex and position->endIndex indicate
* the beginning and ending indices of field number position->field, if such
* a field exists.  This parameter may be NULL, in which case no field
* @param status A pointer to an UErrorCode to receive any errors
* @return The total buffer size needed; if greater than resultLength, the output was truncated.
* @see unum_format
* @see unum_parse
* @see unum_parseDouble
* @see UFieldPosition
* @stable ICU 2.0
*/
U_CAPI int32_t U_EXPORT2 
unum_formatDouble(    const    UNumberFormat*  fmt,
            double          number,
            UChar*          result,
            int32_t         resultLength,
            UFieldPosition  *pos, /* 0 if ignore */
            UErrorCode*     status);

/**
* Parse a string into an integer using a UNumberFormat.
* The string will be parsed according to the UNumberFormat's locale.
* @param fmt The formatter to use.
* @param text The text to parse.
* @param textLength The length of text, or -1 if null-terminated.
* @param parsePos If not 0, on input a pointer to an integer specifying the offset at which
* to begin parsing.  If not 0, on output the offset at which parsing ended.
* @param status A pointer to an UErrorCode to receive any errors
* @return The value of the parsed integer
* @see unum_parseDouble
* @see unum_format
* @see unum_formatDouble
* @stable ICU 2.0
*/
U_CAPI int32_t U_EXPORT2 
unum_parse(    const   UNumberFormat*  fmt,
        const   UChar*          text,
        int32_t         textLength,
        int32_t         *parsePos /* 0 = start */,
        UErrorCode      *status);

/**
* Parse a string into a double using a UNumberFormat.
* The string will be parsed according to the UNumberFormat's locale.
* @param fmt The formatter to use.
* @param text The text to parse.
* @param textLength The length of text, or -1 if null-terminated.
* @param parsePos If not 0, on input a pointer to an integer specifying the offset at which
* to begin parsing.  If not 0, on output the offset at which parsing ended.
* @param status A pointer to an UErrorCode to receive any errors
* @return The value of the parsed double
* @see unum_parse
* @see unum_format
* @see unum_formatDouble
* @stable ICU 2.0
*/
U_CAPI double U_EXPORT2 
unum_parseDouble(    const   UNumberFormat*  fmt,
            const   UChar*          text,
            int32_t         textLength,
            int32_t         *parsePos /* 0 = start */,
            UErrorCode      *status);

/**
* Set the pattern used by an UNumberFormat.
* The pattern should follow the pattern syntax rules.
* @param format The formatter to set.
* @param localized TRUE if the pattern is localized, FALSE otherwise.
* @param pattern The new pattern
* @param parseError  A pointer to UParseError to recieve information about errors
*                    occurred during parsing.
* @param patternLength The length of pattern, or -1 if null-terminated.
* @param status A pointer to an UErrorCode to receive any errors
* @see unum_toPattern
* @stable ICU 2.0
*/
U_CAPI void U_EXPORT2 
unum_applyPattern(          UNumberFormat  *format,
                            UBool          localized,
                    const   UChar          *pattern,
                            int32_t         patternLength,
                            UParseError    *parseError,
                            UErrorCode     *status
                                    );

/**
* Get a locale for which number formatting patterns are available.
* A UNumberFormat in a locale returned by this function will perform the correct
* formatting and parsing for the locale.
* @param index The index of the desired locale.
* @return A locale for which number formatting patterns are available, or 0 if none.
* @see unum_countAvailable
* @stable ICU 2.0
*/
U_CAPI const char* U_EXPORT2 
unum_getAvailable(int32_t index);

/**
* Determine how many locales have number formatting patterns available.
* This function is most useful as determining the loop ending condition for
* calls to \Ref{unum_getAvailable}.
* @return The number of locales for which number formatting patterns are available.
* @see unum_getAvailable
* @stable ICU 2.0
*/
U_CAPI int32_t U_EXPORT2 
unum_countAvailable(void);

/** The possible UNumberFormat numeric attributes @stable ICU 2.0 */
typedef enum UNumberFormatAttribute {
  /** Parse integers only */
  UNUM_PARSE_INT_ONLY,
  /** Use grouping separator */
  UNUM_GROUPING_USED,
  /** Always show decimal point */
  UNUM_DECIMAL_ALWAYS_SHOWN,
  /** Maximum integer digits */
  UNUM_MAX_INTEGER_DIGITS,
  /** Minimum integer digits */
  UNUM_MIN_INTEGER_DIGITS,
  /** Integer digits */
  UNUM_INTEGER_DIGITS,
  /** Maximum fraction digits */
  UNUM_MAX_FRACTION_DIGITS,
  /** Minimum fraction digits */
  UNUM_MIN_FRACTION_DIGITS,
  /** Fraction digits */
  UNUM_FRACTION_DIGITS,
  /** Multiplier */
  UNUM_MULTIPLIER,
  /** Grouping size */
  UNUM_GROUPING_SIZE,
  /** Rounding Mode */
  UNUM_ROUNDING_MODE,
  /** Rounding increment */
  UNUM_ROUNDING_INCREMENT,
  /** The width to which the output of <code>format()</code> is padded. */
  UNUM_FORMAT_WIDTH,
  /** The position at which padding will take place. */
  UNUM_PADDING_POSITION,
  /** Secondary grouping size */
  UNUM_SECONDARY_GROUPING_SIZE
} UNumberFormatAttribute;

/**
* Get a numeric attribute associated with a UNumberFormat.
* An example of a numeric attribute is the number of integer digits a formatter will produce.
* @param fmt The formatter to query.
* @param attr The attribute to query; one of UNUM_PARSE_INT_ONLY, UNUM_GROUPING_USED,
* UNUM_DECIMAL_ALWAYS_SHOWN, UNUM_MAX_INTEGER_DIGITS, UNUM_MIN_INTEGER_DIGITS, UNUM_INTEGER_DIGITS,
* UNUM_MAX_FRACTION_DIGITS, UNUM_MIN_FRACTION_DIGITS, UNUM_FRACTION_DIGITS, UNUM_MULTIPLIER,
* UNUM_GROUPING_SIZE, UNUM_ROUNDING_MODE, UNUM_FORMAT_WIDTH, UNUM_PADDING_POSITION, UNUM_SECONDARY_GROUPING_SIZE.
* @return The value of attr.
* @see unum_setAttribute
* @see unum_getDoubleAttribute
* @see unum_setDoubleAttribute
* @see unum_getTextAttribute
* @see unum_setTextAttribute
* @stable ICU 2.0
*/
U_CAPI int32_t U_EXPORT2 
unum_getAttribute(const UNumberFormat*          fmt,
          UNumberFormatAttribute  attr);

/**
* Set a numeric attribute associated with a UNumberFormat.
* An example of a numeric attribute is the number of integer digits a formatter will produce.
* @param fmt The formatter to set.
* @param attr The attribute to set; one of UNUM_PARSE_INT_ONLY, UNUM_GROUPING_USED,
* UNUM_DECIMAL_ALWAYS_SHOWN, UNUM_MAX_INTEGER_DIGITS, UNUM_MIN_INTEGER_DIGITS, UNUM_INTEGER_DIGITS,
* UNUM_MAX_FRACTION_DIGITS, UNUM_MIN_FRACTION_DIGITS, UNUM_FRACTION_DIGITS, UNUM_MULTIPLIER,
* UNUM_GROUPING_SIZE, UNUM_ROUNDING_MODE, UNUM_FORMAT_WIDTH, UNUM_PADDING_POSITION, UNUM_SECONDARY_GROUPING_SIZE.
* @param newValue The new value of attr.
* @see unum_getAttribute
* @see unum_getDoubleAttribute
* @see unum_setDoubleAttribute
* @see unum_getTextAttribute
* @see unum_setTextAttribute
* @stable ICU 2.0
*/
U_CAPI void U_EXPORT2 
unum_setAttribute(    UNumberFormat*          fmt,
            UNumberFormatAttribute  attr,
            int32_t                 newValue);


/**
* Get a numeric attribute associated with a UNumberFormat.
* An example of a numeric attribute is the number of integer digits a formatter will produce.
* @param fmt The formatter to query.
* @param attr The attribute to query; e.g. UNUM_ROUNDING_INCREMENT.
* @return The value of attr.
* @see unum_getAttribute
* @see unum_setAttribute
* @see unum_setDoubleAttribute
* @see unum_getTextAttribute
* @see unum_setTextAttribute
* @stable ICU 2.0
*/
U_CAPI double U_EXPORT2 
unum_getDoubleAttribute(const UNumberFormat*          fmt,
          UNumberFormatAttribute  attr);

/**
* Set a numeric attribute associated with a UNumberFormat.
* An example of a numeric attribute is the number of integer digits a formatter will produce.
* @param fmt The formatter to set.
* @param attr The attribute to set; e.g. UNUM_ROUNDING_INCREMENT.
* @param newValue The new value of attr.
* @see unum_getAttribute
* @see unum_setAttribute
* @see unum_getDoubleAttribute
* @see unum_getTextAttribute
* @see unum_setTextAttribute
* @stable ICU 2.0
*/
U_CAPI void U_EXPORT2 
unum_setDoubleAttribute(    UNumberFormat*          fmt,
            UNumberFormatAttribute  attr,
            double                 newValue);

/** The possible UNumberFormat text attributes @stable ICU 2.0*/
typedef enum UNumberFormatTextAttribute {
  /** Positive prefix */
  UNUM_POSITIVE_PREFIX,
  /** Positive suffix */
  UNUM_POSITIVE_SUFFIX,
  /** Negative prefix */
  UNUM_NEGATIVE_PREFIX,
  /** Negative suffix */
  UNUM_NEGATIVE_SUFFIX,
  /** The character used to pad to the format width. */
  UNUM_PADDING_CHARACTER,
  /** The ISO currency code */
  UNUM_CURRENCY_CODE
} UNumberFormatTextAttribute;

/**
* Get a text attribute associated with a UNumberFormat.
* An example of a text attribute is the suffix for positive numbers.
* @param fmt The formatter to query.
* @param tag The attribute to query; one of UNUM_POSITIVE_PREFIX, UNUM_POSITIVE_SUFFIX,
* UNUM_NEGATIVE_PREFIX, UNUM_NEGATIVE_SUFFIX
* @param result A pointer to a buffer to receive the attribute.
* @param resultLength The maximum size of result.
* @param status A pointer to an UErrorCode to receive any errors
* @return The total buffer size needed; if greater than resultLength, the output was truncated.
* @see unum_setTextAttribute
* @see unum_getAttribute
* @see unum_setAttribute
* @stable ICU 2.0
*/
U_CAPI int32_t U_EXPORT2 
unum_getTextAttribute(    const    UNumberFormat*                    fmt,
            UNumberFormatTextAttribute      tag,
            UChar*                            result,
            int32_t                            resultLength,
            UErrorCode*                        status);

/**
* Set a text attribute associated with a UNumberFormat.
* An example of a text attribute is the suffix for positive numbers.
* @param fmt The formatter to set.
* @param tag The attribute to set; one of UNUM_POSITIVE_PREFIX, UNUM_POSITIVE_SUFFIX,
* UNUM_NEGATIVE_PREFIX, UNUM_NEGATIVE_SUFFIX
* @param newValue The new value of attr.
* @param newValueLength The length of newValue, or -1 if null-terminated.
* @param status A pointer to an UErrorCode to receive any errors
* @see unum_getTextAttribute
* @see unum_getAttribute
* @see unum_setAttribute
* @stable ICU 2.0
*/
U_CAPI void U_EXPORT2 
unum_setTextAttribute(    UNumberFormat*                    fmt,
            UNumberFormatTextAttribute      tag,
            const    UChar*                            newValue,
            int32_t                            newValueLength,
            UErrorCode                        *status);

/**
* Extract the pattern from a UNumberFormat.
* The pattern will follow the pattern syntax.
* @param fmt The formatter to query.
* @param isPatternLocalized TRUE if the pattern should be localized, FALSE otherwise.
* @param result A pointer to a buffer to receive the pattern.
* @param resultLength The maximum size of result.
* @param status A pointer to an UErrorCode to receive any errors
* @return The total buffer size needed; if greater than resultLength, the output was truncated.
* @stable ICU 2.0
*/
U_CAPI int32_t U_EXPORT2 
unum_toPattern(    const    UNumberFormat*          fmt,
        UBool                  isPatternLocalized,
        UChar*                  result,
        int32_t                 resultLength,
        UErrorCode*             status);

/** The maximum size for a textual number format symbol. @internal*/
#define UNFSYMBOLSMAXSIZE 10

/**
 * Constants for specifying a number format symbol.
 * @stable ICU 2.0
 */
typedef enum UNumberFormatSymbol {
  /** The decimal separator */
  UNUM_DECIMAL_SEPARATOR_SYMBOL,
  /** The grouping separator */
  UNUM_GROUPING_SEPARATOR_SYMBOL,
  /** The pattern separator */
  UNUM_PATTERN_SEPARATOR_SYMBOL,
  /** The percent sign */
  UNUM_PERCENT_SYMBOL,
  /** Zero*/
  UNUM_ZERO_DIGIT_SYMBOL,
  /** Character representing a digit in the pattern */
  UNUM_DIGIT_SYMBOL,
  /** The minus sign */
  UNUM_MINUS_SIGN_SYMBOL,
  /** The plus sign */
  UNUM_PLUS_SIGN_SYMBOL,
  /** The currency symbol */
  UNUM_CURRENCY_SYMBOL,
  /** The international currency symbol */
  UNUM_INTL_CURRENCY_SYMBOL,
  /** The monetary separator */
  UNUM_MONETARY_SEPARATOR_SYMBOL,
  /** The exponential symbol */
  UNUM_EXPONENTIAL_SYMBOL,
  /** Per mill symbol */
  UNUM_PERMILL_SYMBOL,
  /** Escape padding character */
  UNUM_PAD_ESCAPE_SYMBOL,
  /** Infinity symbol */
  UNUM_INFINITY_SYMBOL,
  /** Nan symbol */
  UNUM_NAN_SYMBOL,
  /** count symbol constants */
  UNUM_FORMAT_SYMBOL_COUNT
} UNumberFormatSymbol;

/**
* Get a symbol associated with a UNumberFormat.
* A UNumberFormat uses symbols to represent the special locale-dependent
* characters in a number, for example the percent sign.
* @param fmt The formatter to query.
* @param symbol The UNumberFormatSymbol constant for the symbol to get
* @param buffer The string buffer that will receive the symbol string;
*               if it is NULL, then only the length of the symbol is returned
* @param size The size of the string buffer
* @param status A pointer to an UErrorCode to receive any errors
* @return The length of the symbol; the buffer is not modified if
*         <code>length&gt;=size</code>
* @see unum_setSymbol
* @stable ICU 2.0
*/
U_CAPI int32_t U_EXPORT2
unum_getSymbol(UNumberFormat *fmt,
               UNumberFormatSymbol symbol,
               UChar *buffer,
               int32_t size,
               UErrorCode *status);

/**
* Set a symbol associated with a UNumberFormat.
* A UNumberFormat uses symbols to represent the special locale-dependent
* characters in a number, for example the percent sign.
* @param fmt The formatter to set.
* @param symbol The UNumberFormatSymbol constant for the symbol to set
* @param value The string to set the symbol to
* @param length The length of the string, or -1 for a zero-terminated string
* @param status A pointer to an UErrorCode to receive any errors.
* @see unum_getSymbol
* @stable ICU 2.0
*/
U_CAPI void U_EXPORT2
unum_setSymbol(UNumberFormat *fmt,
               UNumberFormatSymbol symbol,
               const UChar *value,
               int32_t length,
               UErrorCode *status);


/******************* Obsolete API ***************************/
/**
 * TODO: Remove after Aug 2002
 */
#ifdef U_USE_DEPRECATED_FORMAT_API

#if ((U_ICU_VERSION_MAJOR_NUM != 2) || (U_ICU_VERSION_MINOR_NUM != 2))
#   error "ICU version has changed. Please redefine the macros under U_USE_DEPRECATED_FORMAT_API pre-processor definition"
#else 
    static UNumberFormat* 
    unum_openPattern(const UChar* pattern, int32_t patternLength,const char* locale,UErrorCode* status) 
    {
        return unum_open(0,pattern,patternLength,locale,NULL,status);
    }

#   define unum_open_2_2(style,locale,status) unum_open(style, NULL, 0, locale, NULL, status)
#   define unum_applyPattern_2_2(format,localized,pattern,patternLength) unum_applyPattern(format,localized,pattern,patternLength,NULL,NULL)
#endif

#endif
/******************** End ************************************/

#endif /* #if !UCONFIG_NO_FORMATTING */

#endif
