/*
*******************************************************************************
* Copyright (C) 1996-2003, International Business Machines Corporation and others. All Rights Reserved.
*******************************************************************************
*/

#ifndef UDAT_H
#define UDAT_H

#include "unicode/utypes.h"

#if !UCONFIG_NO_FORMATTING

#include "unicode/ucal.h"
#include "unicode/unum.h"
/**
 * \file
 * \brief C API: DateFormat
 *
 * <h2> Date Format C API</h2>
 *
 * Date Format C API  consists of functions that convert dates and
 * times from their internal representations to textual form and back again in a
 * language-independent manner. Converting from the internal representation (milliseconds
 * since midnight, January 1, 1970) to text is known as "formatting," and converting
 * from text to millis is known as "parsing."  We currently define only one concrete
 * structure UDateFormat, which can handle pretty much all normal
 * date formatting and parsing actions.
 * <P>
 * Date Format helps you to format and parse dates for any locale. Your code can
 * be completely independent of the locale conventions for months, days of the
 * week, or even the calendar format: lunar vs. solar.
 * <P>
 * To format a date for the current Locale with default time and date style,
 * use one of the static factory methods:
 * <pre>
 * \code
 *  UErrorCode status = U_ZERO_ERROR;
 *  UChar *myString;
 *  int32_t myStrlen = 0;
 *  UDateFormat* dfmt = udat_open(UDAT_DEFAULT, UDAT_DEFAULT, NULL, NULL, -1, NULL, -1, &status);
 *  myStrlen = udat_format(dfmt, myDate, NULL, myStrlen, NULL, &status);
 *  if (status==U_BUFFER_OVERFLOW_ERROR){
 *      status=U_ZERO_ERROR;
 *      myString=(UChar*)malloc(sizeof(UChar) * (myStrlen+1) );
 *      udat_format(dfmt, myDate, myString, myStrlen+1, NULL, &status);
 *  }
 * \endcode
 * </pre>
 * If you are formatting multiple numbers, it is more efficient to get the
 * format and use it multiple times so that the system doesn't have to fetch the
 * information about the local language and country conventions multiple times.
 * <pre>
 * \code
 *  UErrorCode status = U_ZERO_ERROR;
 *  int32_t i, myStrlen = 0;
 *  UChar* myString;
 *  char buffer[1024];
 *  UDate myDateArr[] = { 0.0, 100000000.0, 2000000000.0 }; // test values
 *  UDateFormat* df = udat_open(UDAT_DEFAULT, UDAT_DEFAULT, NULL, NULL, -1, NULL, 0, &status);
 *  for (i = 0; i < 3; i++) {
 *      myStrlen = udat_format(df, myDateArr[i], NULL, myStrlen, NULL, &status);
 *      if(status == U_BUFFER_OVERFLOW_ERROR){
 *          status = U_ZERO_ERROR;
 *          myString = (UChar*)malloc(sizeof(UChar) * (myStrlen+1) );
 *          udat_format(df, myDateArr[i], myString, myStrlen+1, NULL, &status);
 *          printf("%s\n", u_austrcpy(buffer, myString) );
 *          free(myString);
 *      }
 *  }
 * \endcode
 * </pre>
 * To get specific fields of a date, you can use UFieldPosition to
 * get specific fields.
 * <pre>
 * \code
 *  UErrorCode status = U_ZERO_ERROR;
 *  UFieldPosition pos;
 *  UChar *myString;
 *  int32_t myStrlen = 0;
 *  char buffer[1024];
 *
 *  pos.field = 1;  // Same as the DateFormat::EField enum
 *  UDateFormat* dfmt = udat_open(UDAT_DEFAULT, UDAT_DEFAULT, NULL, -1, NULL, 0, &status);
 *  myStrlen = udat_format(dfmt, myDate, NULL, myStrlen, &pos, &status);
 *  if (status==U_BUFFER_OVERFLOW_ERROR){
 *      status=U_ZERO_ERROR;
 *      myString=(UChar*)malloc(sizeof(UChar) * (myStrlen+1) );
 *      udat_format(dfmt, myDate, myString, myStrlen+1, &pos, &status);
 *  }
 *  printf("date format: %s\n", u_austrcpy(buffer, myString));
 *  buffer[pos.endIndex] = 0;   // NULL terminate the string.
 *  printf("UFieldPosition position equals %s\n", &buffer[pos.beginIndex]);
 * \endcode
 * </pre>
 * To format a date for a different Locale, specify it in the call to
 * udat_open()
 * <pre>
 * \code
 *        UDateFormat* df = udat_open(UDAT_SHORT, UDAT_SHORT, "fr_FR", NULL, -1, NULL, 0, &status);
 * \endcode
 * </pre>
 * You can use a DateFormat API udat_parse() to parse.
 * <pre>
 * \code
 *  UErrorCode status = U_ZERO_ERROR;
 *  int32_t parsepos=0;
 *  UDate myDate = udat_parse(df, myString, u_strlen(myString), &parsepos, &status);
 * \endcode
 * </pre>
 *  You can pass in different options for the arguments for date and time style
 *  to control the length of the result; from SHORT to MEDIUM to LONG to FULL.
 *  The exact result depends on the locale, but generally:
 *  see UDateFormatStyle for more details
 * <ul type=round>
 *   <li>   UDAT_SHORT is completely numeric, such as 12/13/52 or 3:30pm
 *   <li>   UDAT_MEDIUM is longer, such as Jan 12, 1952
 *   <li>   UDAT_LONG is longer, such as January 12, 1952 or 3:30:32pm
 *   <li>   UDAT_FULL is pretty completely specified, such as
 *          Tuesday, April 12, 1952 AD or 3:30:42pm PST.
 * </ul>
 * You can also set the time zone on the format if you wish.
 * <P>
 * You can also use forms of the parse and format methods with Parse Position and
 * UFieldPosition to allow you to
 * <ul type=round>
 *   <li>   Progressively parse through pieces of a string.
 *   <li>   Align any particular field, or find out where it is for selection
 *          on the screen.
 * </ul>
 */

/** A date formatter.
 *  For usage in C programs.
 *  @stable ICU 2.6
 */
typedef void* UDateFormat;

/** The possible date/time format styles 
 *  @stable ICU 2.6
 */
typedef enum UDateFormatStyle {
    /** Full style */
    UDAT_FULL,
    /** Long style */
    UDAT_LONG,
    /** Medium style */
    UDAT_MEDIUM,
    /** Short style */
    UDAT_SHORT,
    /** Default style */
    UDAT_DEFAULT = UDAT_MEDIUM,
    /** No style */
    UDAT_NONE = -1,
    /** for internal API use only */
    UDAT_IGNORE = -2

} UDateFormatStyle;

/**
 * Open a new UDateFormat for formatting and parsing dates and times.
 * A UDateFormat may be used to format dates in calls to \Ref{udat_format},
 * and to parse dates in calls to \Ref{udat_parse}.
 * @param timeStyle The style used to format times; one of UDAT_FULL_STYLE, UDAT_LONG_STYLE,
 * UDAT_MEDIUM_STYLE, UDAT_SHORT_STYLE, or UDAT_DEFAULT_STYLE
 * @param dateStyle The style used to format dates; one of UDAT_FULL_STYLE, UDAT_LONG_STYLE,
 * UDAT_MEDIUM_STYLE, UDAT_SHORT_STYLE, or UDAT_DEFAULT_STYLE
 * @param locale The locale specifying the formatting conventions
 * @param tzID A timezone ID specifying the timezone to use.  If 0, use
 * the default timezone.
 * @param tzIDLength The length of tzID, or -1 if null-terminated.
 * @param pattern A pattern specifying the format to use.
 * @param patternLength The number of characters in the pattern, or -1 if null-terminated.
 * @param status A pointer to an UErrorCode to receive any errors
 * @return A pointer to a UDateFormat to use for formatting dates and times, or 0 if
 * an error occurred.
 * @stable ICU 2.0
 */
U_CAPI UDateFormat* U_EXPORT2 
udat_open(UDateFormatStyle  timeStyle,
          UDateFormatStyle  dateStyle,
          const char        *locale,
          const UChar       *tzID,
          int32_t           tzIDLength,
          const UChar       *pattern,
          int32_t           patternLength,
          UErrorCode        *status);


/**
* Close a UDateFormat.
* Once closed, a UDateFormat may no longer be used.
* @param format The formatter to close.
* @stable ICU 2.0
*/
U_CAPI void U_EXPORT2 
udat_close(UDateFormat* format);

/**
 * Open a copy of a UDateFormat.
 * This function performs a deep copy.
 * @param fmt The format to copy
 * @param status A pointer to an UErrorCode to receive any errors.
 * @return A pointer to a UDateFormat identical to fmt.
 * @stable ICU 2.0
 */
U_CAPI UDateFormat* U_EXPORT2 
udat_clone(const UDateFormat *fmt,
       UErrorCode *status);

/**
* Format a date using an UDateFormat.
* The date will be formatted using the conventions specified in \Ref{udat_open}
* @param format The formatter to use
* @param dateToFormat The date to format
* @param result A pointer to a buffer to receive the formatted number.
* @param resultLength The maximum size of result.
* @param position A pointer to a UFieldPosition.  On input, position->field
* is read.  On output, position->beginIndex and position->endIndex indicate
* the beginning and ending indices of field number position->field, if such
* a field exists.  This parameter may be NULL, in which case no field
* position data is returned.
* @param status A pointer to an UErrorCode to receive any errors
* @return The total buffer size needed; if greater than resultLength, the output was truncated.
* @see udat_parse
* @see UFieldPosition
* @stable ICU 2.0
*/
U_CAPI int32_t U_EXPORT2 
udat_format(    const    UDateFormat*    format,
                        UDate           dateToFormat,
                        UChar*          result,
                        int32_t         resultLength,
                        UFieldPosition* position,
                        UErrorCode*     status);

/**
* Parse a string into an date/time using a UDateFormat.
* The date will be parsed using the conventions specified in \Ref{udat_open}
* @param format The formatter to use.
* @param text The text to parse.
* @param textLength The length of text, or -1 if null-terminated.
* @param parsePos If not 0, on input a pointer to an integer specifying the offset at which
* to begin parsing.  If not 0, on output the offset at which parsing ended.
* @param status A pointer to an UErrorCode to receive any errors
* @return The value of the parsed date/time
* @see udat_format
* @stable ICU 2.0
*/
U_CAPI UDate U_EXPORT2 
udat_parse(    const    UDateFormat*    format,
            const    UChar*          text,
                    int32_t         textLength,
                    int32_t         *parsePos,
                    UErrorCode      *status);

/**
* Parse a string into an date/time using a UDateFormat.
* The date will be parsed using the conventions specified in \Ref{udat_open}
* @param format The formatter to use.
* @param calendar The calendar in which to store the parsed data.
* @param text The text to parse.
* @param textLength The length of text, or -1 if null-terminated.
* @param parsePos If not 0, on input a pointer to an integer specifying the offset at which
* to begin parsing.  If not 0, on output the offset at which parsing ended.
* @param status A pointer to an UErrorCode to receive any errors
* @see udat_format
* @stable ICU 2.0
*/
U_CAPI void U_EXPORT2 
udat_parseCalendar(const    UDateFormat*    format,
                            UCalendar*      calendar,
                   const    UChar*          text,
                            int32_t         textLength,
                            int32_t         *parsePos,
                            UErrorCode      *status);

/**
* Determine if an UDateFormat will perform lenient parsing.
* With lenient parsing, the parser may use heuristics to interpret inputs that do not
* precisely match the pattern. With strict parsing, inputs must match the pattern.
* @param fmt The formatter to query
* @return TRUE if fmt is set to perform lenient parsing, FALSE otherwise.
* @see udat_setLenient
* @stable ICU 2.0
*/
U_CAPI UBool U_EXPORT2 
udat_isLenient(const UDateFormat* fmt);

/**
* Specify whether an UDateFormat will perform lenient parsing.
* With lenient parsing, the parser may use heuristics to interpret inputs that do not
* precisely match the pattern. With strict parsing, inputs must match the pattern.
* @param fmt The formatter to set
* @param isLenient TRUE if fmt should perform lenient parsing, FALSE otherwise.
* @see dat_isLenient
* @stable ICU 2.0
*/
U_CAPI void U_EXPORT2 
udat_setLenient(    UDateFormat*    fmt,
                    UBool          isLenient);

/**
* Get the UCalendar associated with an UDateFormat.
* A UDateFormat uses a UCalendar to convert a raw value to, for example,
* the day of the week.
* @param fmt The formatter to query.
* @return A pointer to the UCalendar used by fmt.
* @see udat_setCalendar
* @stable ICU 2.0
*/
U_CAPI const UCalendar* U_EXPORT2 
udat_getCalendar(const UDateFormat* fmt);

/**
* Set the UCalendar associated with an UDateFormat.
* A UDateFormat uses a UCalendar to convert a raw value to, for example,
* the day of the week.
* @param fmt The formatter to set.
* @param calendarToSet A pointer to an UCalendar to be used by fmt.
* @see udat_setCalendar
* @stable ICU 2.0
*/
U_CAPI void U_EXPORT2 
udat_setCalendar(            UDateFormat*    fmt,
                    const   UCalendar*      calendarToSet);

/**
* Get the UNumberFormat associated with an UDateFormat.
* A UDateFormat uses a UNumberFormat to format numbers within a date,
* for example the day number.
* @param fmt The formatter to query.
* @return A pointer to the UNumberFormat used by fmt to format numbers.
* @see udat_setNumberFormat
* @stable ICU 2.0
*/
U_CAPI const UNumberFormat* U_EXPORT2 
udat_getNumberFormat(const UDateFormat* fmt);

/**
* Set the UNumberFormat associated with an UDateFormat.
* A UDateFormat uses a UNumberFormat to format numbers within a date,
* for example the day number.
* @param fmt The formatter to set.
* @param numberFormatToSet A pointer to the UNumberFormat to be used by fmt to format numbers.
* @see udat_getNumberFormat
* @stable ICU 2.0
*/
U_CAPI void U_EXPORT2 
udat_setNumberFormat(            UDateFormat*    fmt,
                        const   UNumberFormat*  numberFormatToSet);

/**
* Get a locale for which date/time formatting patterns are available.
* A UDateFormat in a locale returned by this function will perform the correct
* formatting and parsing for the locale.
* @param index The index of the desired locale.
* @return A locale for which date/time formatting patterns are available, or 0 if none.
* @see udat_countAvailable
* @stable ICU 2.0
*/
U_CAPI const char* U_EXPORT2 
udat_getAvailable(int32_t index);

/**
* Determine how many locales have date/time  formatting patterns available.
* This function is most useful as determining the loop ending condition for
* calls to \Ref{udat_getAvailable}.
* @return The number of locales for which date/time formatting patterns are available.
* @see udat_getAvailable
* @stable ICU 2.0
*/
U_CAPI int32_t U_EXPORT2 
udat_countAvailable(void);

/**
* Get the year relative to which all 2-digit years are interpreted.
* For example, if the 2-digit start year is 2100, the year 99 will be
* interpreted as 2199.
* @param fmt The formatter to query.
* @param status A pointer to an UErrorCode to receive any errors
* @return The year relative to which all 2-digit years are interpreted.
* @see udat_Set2DigitYearStart
* @stable ICU 2.0
*/
U_CAPI UDate U_EXPORT2 
udat_get2DigitYearStart(    const   UDateFormat     *fmt,
                                    UErrorCode      *status);

/**
* Set the year relative to which all 2-digit years will be interpreted.
* For example, if the 2-digit start year is 2100, the year 99 will be
* interpreted as 2199.
* @param fmt The formatter to set.
* @param d The year relative to which all 2-digit years will be interpreted.
* @param status A pointer to an UErrorCode to receive any errors
* @see udat_Set2DigitYearStart
* @stable ICU 2.0
*/
U_CAPI void U_EXPORT2 
udat_set2DigitYearStart(    UDateFormat     *fmt,
                            UDate           d,
                            UErrorCode      *status);

/**
* Extract the pattern from a UDateFormat.
* The pattern will follow the pattern syntax rules.
* @param fmt The formatter to query.
* @param localized TRUE if the pattern should be localized, FALSE otherwise.
* @param result A pointer to a buffer to receive the pattern.
* @param resultLength The maximum size of result.
* @param status A pointer to an UErrorCode to receive any errors
* @return The total buffer size needed; if greater than resultLength, the output was truncated.
* @see udat_applyPattern
* @stable ICU 2.0
*/
U_CAPI int32_t U_EXPORT2 
udat_toPattern(    const   UDateFormat     *fmt,
                        UBool          localized,
                        UChar           *result,
                        int32_t         resultLength,
                        UErrorCode      *status);

/**
* Set the pattern used by an UDateFormat.
* The pattern should follow the pattern syntax rules.
* @param format The formatter to set.
* @param localized TRUE if the pattern is localized, FALSE otherwise.
* @param pattern The new pattern
* @param patternLength The length of pattern, or -1 if null-terminated.
* @see udat_toPattern
* @stable ICU 2.0
*/
U_CAPI void U_EXPORT2 
udat_applyPattern(            UDateFormat     *format,
                            UBool          localized,
                    const   UChar           *pattern,
                            int32_t         patternLength);

/** 
 * The possible types of date format symbols 
 * @stable ICU 2.6
 */
typedef enum UDateFormatSymbolType {
    /** The era names, for example AD */
    UDAT_ERAS,
    /** The month names, for example February */
    UDAT_MONTHS,
    /** The short month names, for example Feb. */
    UDAT_SHORT_MONTHS,
    /** The weekday names, for example Monday */
    UDAT_WEEKDAYS,
    /** The short weekday names, for example Mon. */
    UDAT_SHORT_WEEKDAYS,
    /** The AM/PM names, for example AM */
    UDAT_AM_PMS,
    /** The localized characters */
    UDAT_LOCALIZED_CHARS
} UDateFormatSymbolType;

struct UDateFormatSymbols;
/** Date format symbols.
 *  For usage in C programs.
 *  @stable ICU 2.6
 */
typedef struct UDateFormatSymbols UDateFormatSymbols;

/**
* Get the symbols associated with an UDateFormat.
* The symbols are what a UDateFormat uses to represent locale-specific data,
* for example month or day names.
* @param fmt The formatter to query.
* @param type The type of symbols to get.  One of UDAT_ERAS, UDAT_MONTHS, UDAT_SHORT_MONTHS,
* UDAT_WEEKDAYS, UDAT_SHORT_WEEKDAYS, UDAT_AM_PMS, or UDAT_LOCALIZED_CHARS
* @param index The desired symbol of type type.
* @param result A pointer to a buffer to receive the pattern.
* @param resultLength The maximum size of result.
* @param status A pointer to an UErrorCode to receive any errors
* @return The total buffer size needed; if greater than resultLength, the output was truncated.
* @see udat_countSymbols
* @see udat_setSymbols
* @stable ICU 2.0
*/
U_CAPI int32_t U_EXPORT2 
udat_getSymbols(const   UDateFormat             *fmt,
                        UDateFormatSymbolType   type,
                        int32_t                 index,
                        UChar                   *result,
                        int32_t                 resultLength,
                        UErrorCode              *status);

/**
* Count the number of particular symbols for an UDateFormat.
* This function is most useful as for detemining the loop termination condition
* for calls to \Ref{udat_getSymbols}.
* @param fmt The formatter to query.
* @param type The type of symbols to count.  One of UDAT_ERAS, UDAT_MONTHS, UDAT_SHORT_MONTHS,
* UDAT_WEEKDAYS, UDAT_SHORT_WEEKDAYS, UDAT_AM_PMS, or UDAT_LOCALIZED_CHARS
* @return The number of symbols of type type.
* @see udat_getSymbols
* @see udat_setSymbols
* @stable ICU 2.0
*/
U_CAPI int32_t U_EXPORT2 
udat_countSymbols(    const    UDateFormat                *fmt,
                            UDateFormatSymbolType    type);

/**
* Set the symbols associated with an UDateFormat.
* The symbols are what a UDateFormat uses to represent locale-specific data,
* for example month or day names.
* @param format The formatter to set
* @param type The type of symbols to set.  One of UDAT_ERAS, UDAT_MONTHS, UDAT_SHORT_MONTHS,
* UDAT_WEEKDAYS, UDAT_SHORT_WEEKDAYS, UDAT_AM_PMS, or UDAT_LOCALIZED_CHARS
* @param index The index of the symbol to set of type type.
* @param value The new value
* @param valueLength The length of value, or -1 if null-terminated
* @param status A pointer to an UErrorCode to receive any errors
* @see udat_getSymbols
* @see udat_countSymbols
* @stable ICU 2.0
*/
U_CAPI void U_EXPORT2 
udat_setSymbols(    UDateFormat             *format,
                    UDateFormatSymbolType   type,
                    int32_t                 index,
                    UChar                   *value,
                    int32_t                 valueLength,
                    UErrorCode              *status);

/********************* Obsolete API ************************************/
/**
 * TODO: Remove after Aug 2002
 */
#ifdef U_USE_DEPRECATED_FORMAT_API
#if ((U_ICU_VERSION_MAJOR_NUM != 2) || (U_ICU_VERSION_MINOR_NUM != 2))
#   error "ICU version has changed. Please redefine the macros under U_USE_DEPRECATED_FORMAT_API pre-processor definition"
#else 
    static UDateFormat*
    udat_openPattern(const UChar* pattern,int32_t patternLength,const char* locale,UErrorCode *status)
    {
        return udat_open(UDAT_IGNORE,UDAT_IGNORE,locale,NULL,0,pattern,patternLength,status);
    }

#   define udat_open_2_2(timeStyle,dateStyle,locale,tzId,tzIdLength,status) udat_open(timeStyle,dateStyle,locale,tzId,tzIdLength,NULL,0,status)
#endif
#endif
/********************* End **********************************************/

#endif /* #if !UCONFIG_NO_FORMATTING */

#endif
