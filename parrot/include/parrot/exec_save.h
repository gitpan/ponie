/*
 * exec.h
 *
 * CVS Info
 *    $Id: exec_save.h,v 1.1 2003/07/25 20:44:29 grunblatt Exp $
 * Overview:
 *    Exec header file.
 * History:
 *      Initial version by Daniel Grunblatt on 2003.6.9
 * Notes:
 * References:
 */

#ifndef EXEC_SAVE_H_GUARD
#  define EXEC_SAVE_H_GUARD

void Parrot_exec_save(Parrot_exec_objfile_t *obj, const char *file);

#endif /* EXEC_SAVE_H_GUARD */

/*
 * Local variables:
 * c-indentation-style: bsd
 * c-basic-offset: 4
 * indent-tabs-mode: nil
 * End:
 *
 * vim: expandtab shiftwidth=4:
 */