/* dynext.h
*
* $Id: dynext.h,v 1.4 2003/09/30 09:29:41 leo Exp $
*
*   Parrot dynamic extensions
*/

#if !defined(DYNEXT_H_GUARD)
#define DYNEXT_H_GUARD


/* dynamic lib/oplib/PMC loading */
PMC *Parrot_load_lib(Interp *interpreter, STRING *lib, PMC *initializer);

#endif

