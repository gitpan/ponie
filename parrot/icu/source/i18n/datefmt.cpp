/*
*******************************************************************************
* Copyright (C) 1997-2003, International Business Machines Corporation and    *
* others. All Rights Reserved.                                                *
*******************************************************************************
*
* File DATEFMT.CPP
*
* Modification History:
*
*   Date        Name        Description
*   02/19/97    aliu        Converted from java.
*   03/31/97    aliu        Modified extensively to work with 50 locales.
*   04/01/97    aliu        Added support for centuries.
*   08/12/97    aliu        Fixed operator== to use Calendar::equivalentTo.
*    07/20/98    stephen        Changed ParsePosition initialization
********************************************************************************
*/

#include "unicode/utypes.h"

#if !UCONFIG_NO_FORMATTING

#include "unicode/resbund.h"
#include "unicode/datefmt.h"
#include "unicode/smpdtfmt.h"

// *****************************************************************************
// class DateFormat
// *****************************************************************************

U_NAMESPACE_BEGIN

DateFormat::DateFormat()
:   fCalendar(0),
    fNumberFormat(0)
{
}

//----------------------------------------------------------------------

DateFormat::DateFormat(const DateFormat& other)
:   Format(other),
    fCalendar(0),
    fNumberFormat(0)
{
    *this = other;
}

//----------------------------------------------------------------------

DateFormat& DateFormat::operator=(const DateFormat& other)
{
    if (this != &other)
    {
        delete fCalendar;
        delete fNumberFormat;
        if(other.fCalendar) {
          fCalendar = other.fCalendar->clone();
        } else {
          fCalendar = NULL;
        }
        if(other.fNumberFormat) {
          fNumberFormat = (NumberFormat*)other.fNumberFormat->clone();
        } else {
          fNumberFormat = NULL;
        }
    }
    return *this;
}

//----------------------------------------------------------------------

DateFormat::~DateFormat()
{
    delete fCalendar;
    delete fNumberFormat;
}

//----------------------------------------------------------------------

UBool
DateFormat::operator==(const Format& other) const
{
    // This protected comparison operator should only be called by subclasses
    // which have confirmed that the other object being compared against is
    // an instance of a sublcass of DateFormat.  THIS IS IMPORTANT.

    // We only dereference this pointer after we have confirmed below that
    // 'other' is a DateFormat subclass.
    DateFormat* fmt = (DateFormat*)&other;

    return (this == fmt) ||
        ((getDynamicClassID() == other.getDynamicClassID()) &&
         fCalendar&&(fCalendar->isEquivalentTo(*fmt->fCalendar)) &&
         (fNumberFormat&&(*fNumberFormat == *fmt->fNumberFormat)) );
}

//----------------------------------------------------------------------

UnicodeString&
DateFormat::format(const Formattable& obj,
                   UnicodeString& appendTo,
                   FieldPosition& fieldPosition,
                   UErrorCode& status) const
{
    if (U_FAILURE(status)) return appendTo;

    // if the type of the Formattable is double or long, treat it as if it were a Date
    UDate date = 0;
    switch (obj.getType())
    {
    case Formattable::kDate:
        date = obj.getDate();
        break;
    case Formattable::kDouble:
        date = (UDate)obj.getDouble();
        break;
    case Formattable::kLong:
        date = (UDate)obj.getLong();
        break;
    default:
        status = U_ILLEGAL_ARGUMENT_ERROR;
        return appendTo;
    }

    // Is this right?
    //if (fieldPosition.getBeginIndex() == fieldPosition.getEndIndex())
    //  status = U_ILLEGAL_ARGUMENT_ERROR;

    return format(date, appendTo, fieldPosition);
}

//----------------------------------------------------------------------

UnicodeString&
DateFormat::format(UDate date, UnicodeString& appendTo, FieldPosition& fieldPosition) const {
    if (fCalendar != NULL) {
        // Use our calendar instance
        UErrorCode ec = U_ZERO_ERROR;
        fCalendar->setTime(date, ec);
        if (U_SUCCESS(ec)) {
            return format(*fCalendar, appendTo, fieldPosition);
        }
    }
    return appendTo;
}

//----------------------------------------------------------------------

UnicodeString&
DateFormat::format(UDate date, UnicodeString& appendTo) const
{
    // Note that any error information is just lost.  That's okay
    // for this convenience method.
    FieldPosition fpos(0);
    return format(date, appendTo, fpos);
}

//----------------------------------------------------------------------

UDate
DateFormat::parse(const UnicodeString& text,
                  ParsePosition& pos) const
{
    if (fCalendar != NULL) {
        int32_t start = pos.getIndex();
        fCalendar->clear();
        parse(text, *fCalendar, pos);
        if (pos.getIndex() != start) {
            UErrorCode ec = U_ZERO_ERROR;
            UDate d = fCalendar->getTime(ec);
            if (U_SUCCESS(ec)) {
                return d; // Successful function exit
            }
            // We arrive here if fCalendar is non-lenient and there
            // is an out-of-range field.  We don't know which field
            // was illegal so we set the error index to the start.
            pos.setIndex(start);
            pos.setErrorIndex(start);
        }
    }
    return 0; // Error return UDate is 0 (the epoch)
}

//----------------------------------------------------------------------

UDate
DateFormat::parse(const UnicodeString& text,
                  UErrorCode& status) const
{
    if (U_FAILURE(status)) return 0;

    ParsePosition pos(0);
    UDate result = parse(text, pos);
    if (pos.getIndex() == 0) status = U_ILLEGAL_ARGUMENT_ERROR;
    return result;
}

//----------------------------------------------------------------------

void
DateFormat::parseObject(const UnicodeString& source,
                        Formattable& result,
                        ParsePosition& pos) const
{
    result.setDate(parse(source, pos));
}

//----------------------------------------------------------------------

DateFormat*
DateFormat::createTimeInstance(DateFormat::EStyle style,
                               const Locale& aLocale)
{
    return create(style, kNone, aLocale);
}

//----------------------------------------------------------------------

DateFormat*
DateFormat::createDateInstance(DateFormat::EStyle style,
                               const Locale& aLocale)
{
  // +4 to set the correct index for getting data out of
  // LocaleElements.
  if(style != kNone)
  {
    style = (EStyle) (style + kDateOffset);
  }
  return create(kNone, (EStyle) (style), aLocale);
}

//----------------------------------------------------------------------

DateFormat*
DateFormat::createDateTimeInstance(EStyle dateStyle,
                                   EStyle timeStyle,
                                   const Locale& aLocale)
{
  if(dateStyle != kNone)
  {
    dateStyle = (EStyle) (dateStyle + kDateOffset);
  }
  return create(timeStyle, dateStyle, aLocale);
}

//----------------------------------------------------------------------

DateFormat*
DateFormat::createInstance()
{
    return create(kShort, (EStyle) (kShort + kDateOffset), Locale::getDefault());
}

//----------------------------------------------------------------------

DateFormat*
DateFormat::create(EStyle timeStyle, EStyle dateStyle, const Locale& locale)
{
    // Try to create a SimpleDateFormat of the desired style.
    UErrorCode status = U_ZERO_ERROR;
    SimpleDateFormat *f = new SimpleDateFormat(timeStyle, dateStyle, locale, status);
    if (U_SUCCESS(status)) return f;
    delete f;

    // If that fails, try to create a format using the default pattern and
    // the DateFormatSymbols for this locale.
    status = U_ZERO_ERROR;
    f = new SimpleDateFormat(locale, status);
    if (U_SUCCESS(status)) return f;
    delete f;

    // This should never really happen, because the preceding constructor
    // should always succeed.  If the resource data is unavailable, a last
    // resort object should be returned.
    return 0;
}

//----------------------------------------------------------------------

const Locale*
DateFormat::getAvailableLocales(int32_t& count)
{
    // Get the list of installed locales.
    // Even if root has the correct date format for this locale,
    // it's still a valid locale (we don't worry about data fallbacks).
    return Locale::getAvailableLocales(count);
}

//----------------------------------------------------------------------

void
DateFormat::adoptCalendar(Calendar* newCalendar)
{
    delete fCalendar;
    fCalendar = newCalendar;
}

//----------------------------------------------------------------------
void
DateFormat::setCalendar(const Calendar& newCalendar)
{
    adoptCalendar(newCalendar.clone());
}

//----------------------------------------------------------------------

const Calendar*
DateFormat::getCalendar() const
{
    return fCalendar;
}

//----------------------------------------------------------------------

void
DateFormat::adoptNumberFormat(NumberFormat* newNumberFormat)
{
    delete fNumberFormat;
    fNumberFormat = newNumberFormat;
    newNumberFormat->setParseIntegerOnly(TRUE);
}
//----------------------------------------------------------------------

void
DateFormat::setNumberFormat(const NumberFormat& newNumberFormat)
{
    adoptNumberFormat((NumberFormat*)newNumberFormat.clone());
}

//----------------------------------------------------------------------

const NumberFormat*
DateFormat::getNumberFormat() const
{
    return fNumberFormat;
}

//----------------------------------------------------------------------

void
DateFormat::adoptTimeZone(TimeZone* zone)
{
    fCalendar->adoptTimeZone(zone);
}
//----------------------------------------------------------------------

void
DateFormat::setTimeZone(const TimeZone& zone)
{
    fCalendar->setTimeZone(zone);
}

//----------------------------------------------------------------------

const TimeZone&
DateFormat::getTimeZone() const
{
    return fCalendar->getTimeZone();
}

//----------------------------------------------------------------------

void
DateFormat::setLenient(UBool lenient)
{
    fCalendar->setLenient(lenient);
}

//----------------------------------------------------------------------

UBool
DateFormat::isLenient() const
{
    return fCalendar->isLenient();
}

U_NAMESPACE_END

#endif /* #if !UCONFIG_NO_FORMATTING */

//eof
