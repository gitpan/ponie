/* perltypes.h
 *  Copyright: 2001-2003 The Perl Foundation.  All Rights Reserved.
 *  CVS Info
 *     $Id: perltypes.h,v 1.6 2003/07/21 18:00:42 chromatic Exp $
 *  Overview:
 *     Header to be shared among Perl PMC classes
 *  Data Structure and Algorithms:
 *  History:
 *  Notes:
 *  References:
 */

/*
 * Change PMC "thing" to scalar type "type", updating vtable and flags
 * as necessary.
 */

#define DONT_USE_CHANGE_TYPE(thing, type) { \
    if ((thing)->vtable == &Parrot_base_vtables[enum_class_PerlString]) { \
	PObj_is_buffer_ptr_CLEAR(thing); \
    } \
    (thing)->vtable = &Parrot_base_vtables[enum_class_ ## type]; \
    if (enum_class_ ## type == enum_class_PerlString) { \
	PObj_is_buffer_ptr_SET(thing); \
    } \
}

/*
 * Local variables:
 * c-indentation-style: bsd
 * c-basic-offset: 4
 * indent-tabs-mode: nil
 * End:
 *
 * vim: expandtab shiftwidth=4:
*/