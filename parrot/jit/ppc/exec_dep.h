/*
 * exec_dep.h
 *
 * CVS Info
 *    $Id: exec_dep.h,v 1.6 2003/08/11 18:23:32 grunblatt Exp $
 * Overview:
 *    PPC dependent functions to emit an executable.
 * History:
 *      Initial version by Daniel Grunblatt on 2003.6.9
 * Notes:
 * References:
 */

#ifndef EXEC_DEP_H_GUARD
#  define EXEC_DEP_H_GUARD

#  ifdef JIT_CGP

void
Parrot_exec_normal_op(Parrot_jit_info_t *jit_info,
                     struct Parrot_Interp * interpreter)
{
}

#  else /* JIT_CGP */

void
Parrot_exec_normal_op(Parrot_jit_info_t *jit_info,
                     struct Parrot_Interp * interpreter)
{
    add_disp(jit_info->native_ptr, r3,
        ((long)jit_info->cur_op - (long)interpreter->code->byte_code));
    jit_emit_mov_rr(jit_info->native_ptr, r4, r13);

    Parrot_exec_add_text_rellocation(jit_info->objfile,
        jit_info->native_ptr, RTYPE_FUNC,
            interpreter->op_info_table[*jit_info->cur_op].func_name, 0);
    _emit_bx(jit_info->native_ptr, 1, 0);
}

#  endif /* JIT_CGP */

void
Parrot_exec_cpcf_op(Parrot_jit_info_t *jit_info,
                   struct Parrot_Interp * interpreter)
{
    Parrot_exec_normal_op(jit_info, interpreter);
    jit_emit_sub_rrr(jit_info->native_ptr, r3, r3, r15);
    jit_emit_add_rrr(jit_info->native_ptr, r3, r14, r3);
    jit_emit_lwz(jit_info->native_ptr, r3, 0, r3);
    jit_emit_mtlr(jit_info->native_ptr, r3);
    jit_emit_blr(jit_info->native_ptr);
}

void
Parrot_exec_restart_op(Parrot_jit_info_t *jit_info,
                       struct Parrot_Interp * interpreter)
{
}

void
Parrot_exec_emit_mov_mr(struct Parrot_Interp * interpreter, char *mem, int reg)
{
    Parrot_jit_emit_mov_mr(interpreter, mem, reg);
}

void
Parrot_exec_emit_mov_rm(struct Parrot_Interp * interpreter, int reg, char *mem)
{
    Parrot_jit_emit_mov_rm(interpreter, reg, mem);
}

void
Parrot_exec_emit_mov_mr_n(struct Parrot_Interp * interpreter, char *mem,int reg)
{
    Parrot_jit_emit_mov_mr_n(interpreter, mem, reg);
}

void
Parrot_exec_emit_mov_rm_n(struct Parrot_Interp * interpreter, int reg,char *mem)
{
    Parrot_jit_emit_mov_rm_n(interpreter, reg, mem);
}

/* Assign the offset of the progra_code */
static void
offset_fixup(Parrot_exec_objfile_t *obj)
{
    int i,j;

    for (i = 0; i < obj->data_count; i++) {
#  ifdef EXEC_MACH_O
        obj->symbol_table[i].value = obj->text.size;
#  endif
        for (j = 0; j < i; j++) 
            obj->symbol_table[i].value += obj->data_size[j];
    }
}

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
