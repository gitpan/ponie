/* exit.h
 *  Copyright: 2001-2003 The Perl Foundation.  All Rights Reserved.
 *  CVS Info
 *     $Id: exit.h,v 1.2 2003/07/21 18:00:42 chromatic Exp $
 *  Overview:
 *     
 *  Data Structure and Algorithms:
 *  History:
 *  Notes:
 *  References:
 *      exit.c
 */

#if !defined(PARROT_EXIT_H_GUARD)
#define PARROT_EXIT_H_GUARD

int Parrot_on_exit(void (*function)(int , void *), void *arg);
void Parrot_exit(int status);

#endif

/*
 * Local variables:
 * c-indentation-style: bsd
 * c-basic-offset: 4
 * indent-tabs-mode: nil
 * End:
 *
 * vim: expandtab shiftwidth=4:
 */
