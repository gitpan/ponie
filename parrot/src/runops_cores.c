/*
Copyright: 2001-2003 The Perl Foundation.  All Rights Reserved.
$Id: runops_cores.c,v 1.45 2004/03/09 17:03:35 leo Exp $

=head1 NAME

src/runops_cores.c - Run Loops

=head1 DESCRIPTION

This file implements the various run loops for the interpreter. A slow
one with bounds checking, tracing and (optional) profiling, and a fast
one without. There's also one which uses computed computed C<goto>,
which enables the faster dispatch of operations.

=head2 Functions

=over 4

=cut

*/

#include "parrot/runops_cores.h"
#include "parrot/trace.h"

#include "parrot/interp_guts.h"

#ifdef HAVE_COMPUTED_GOTO
#  include "parrot/oplib/core_ops_cg.h"
#endif

/*

=item C<opcode_t *
runops_fast_core(struct Parrot_Interp *interpreter, opcode_t *pc)>

Runs the Parrot operations starting at C<pc> until there are no more
operations.

No bounds checking, profiling or tracing is performed.

=cut

*/

opcode_t *
runops_fast_core(struct Parrot_Interp *interpreter, opcode_t *pc)
{
    while (pc) {
        DO_OP(pc, interpreter);
    }
    return pc;
}

/*

=item C<opcode_t *
runops_cgoto_core(struct Parrot_Interp *interpreter, opcode_t *pc)>

Runs the Parrot operations starting at C<pc> until there are no more
operations, using the computed C<goto> core.

No bounds checking, profiling or tracing is performed.

If computed C<goto> is not available then Parrot exits with exit code 1.

=cut

*/

opcode_t *
runops_cgoto_core(struct Parrot_Interp *interpreter, opcode_t *pc)
{
#ifdef HAVE_COMPUTED_GOTO
    pc = cg_core(pc, interpreter);
    return pc;
#else
    PIO_eprintf(interpreter,
            "Computed goto unavailable in this configuration.\n");
    Parrot_exit(1);
    return NULL;
#endif
}

/*

=item C<opcode_t *
runops_slow_core(struct Parrot_Interp *interpreter, opcode_t *pc)>

Runs the Parrot operations starting at C<pc> until there are no more
operations, with tracing and bounds checking enabled.

Note that using an C<extern> interpreter for tracing is currently broken
because C<Parrot_destroy()> fails to cleanup of C<ctx> correctly.

=cut

*/

opcode_t *
runops_slow_core(struct Parrot_Interp *interpreter, opcode_t *pc)
{
#ifdef USE_TRACE_INTERP
    Interp * trace_i;
    struct Parrot_Context *trace_ctx;
#endif
    opcode_t *opc, *ostart, *oend;
    static size_t dod, gc;

#ifdef code_start
#  undef code_start
#endif
#ifdef code_end
#  undef code_end
#endif

#define  code_start interpreter->code->byte_code
#define  code_end   (interpreter->code->byte_code + \
        interpreter->code->cur_cs->base.size)


#ifdef USE_TRACE_INTERP
    if (Interp_flags_TEST(interpreter, PARROT_TRACE_FLAG)) {
        trace_i = make_interpreter(interpreter, NO_FLAGS);
        Parrot_init(trace_i);
        /* remeber old context */
        trace_ctx = mem_sys_allocate(sizeof(struct Parrot_Context));
        mem_sys_memcopy(trace_ctx, &trace_i->ctx,
                sizeof(struct Parrot_Context));
        /* copy in current */
        mem_sys_memcopy(&trace_i->ctx, &interpreter->ctx,
                sizeof(struct Parrot_Context));
        trace_i->code = interpreter->code;
        Interp_flags_SET(trace_i, PARROT_EXTERN_CODE_FLAG);
    }
#endif

    dod = interpreter->dod_runs;
    gc = interpreter->collect_runs;
    while (pc) {/* && pc >= code_start && pc < code_end) {*/
        interpreter->cur_pc = pc;
        opc = pc;
        ostart = code_start;
        oend = code_end;

        DO_OP(pc, interpreter);

        if (Interp_flags_TEST(interpreter, PARROT_TRACE_FLAG)) {
#ifdef USE_TRACE_INTERP
            mem_sys_memcopy(&trace_i->ctx, &interpreter->ctx,
                    sizeof(struct Parrot_Context));
            trace_op(trace_i, ostart, oend, opc);
#else
            trace_op(interpreter, ostart, oend, opc);
#endif
            if (dod != interpreter->dod_runs) {
                dod = interpreter->dod_runs;
                PIO_printf(interpreter, "       DOD\n");
            }
            if (gc != interpreter->collect_runs) {
                gc = interpreter->collect_runs;
                PIO_printf(interpreter, "       GC\n");
            }
        }
    }
#ifdef USE_TRACE_INTERP
    if (Interp_flags_TEST(interpreter, PARROT_TRACE_FLAG)) {
        /* restore trace context */
        mem_sys_memcopy(&trace_i->ctx, trace_ctx,
                sizeof(struct Parrot_Context));
        mem_sys_free(trace_ctx);
    }
#endif

    /*    if (pc && (pc < code_start || pc >= code_end)) {
        internal_exception(INTERP_ERROR,
       "Error: Control left bounds of byte-code block (now at location %d)!\n",
       (int)(pc - code_start));
       }*/
#undef code_start
#undef code_end
    return pc;
}

/*

=item C<opcode_t *
runops_profile_core(struct Parrot_Interp *interpreter, opcode_t *pc)>

Runs the Parrot operations starting at C<pc> until there are no more
operations, with tracing, bounds checking and profiling enabled.

=cut

*/

opcode_t *
runops_profile_core(struct Parrot_Interp *interpreter, opcode_t *pc)
{
    opcode_t cur_op, old_op;
    RunProfile *profile = interpreter->profile;

    old_op = profile->cur_op;
    /*
     * if we are reentering the runloop:
     * - remember old op and calc time till now
     */
    if (old_op) {
        profile->data[old_op].time +=
            Parrot_floatval_time() - profile->starttime;
    }

    while (pc) {/* && pc >= code_start && pc < code_end) */
        interpreter->cur_pc = pc;
        profile->cur_op = cur_op = *pc + PARROT_PROF_EXTRA;
        profile->data[cur_op].numcalls++;
        profile->starttime = Parrot_floatval_time();

        DO_OP(pc, interpreter);

        /* profile->cur_op may be different, if exception was thrown */
        profile->data[profile->cur_op].time +=
            Parrot_floatval_time() - profile->starttime;
    }
    if (old_op) {
        /* old opcode continues */
        profile->starttime = Parrot_floatval_time();
        profile->cur_op = old_op;
    }
    return pc;
}


/*

=back

=head1 SEE ALSO

F<include/parrot/runops_cores.h>.

=cut

*/

/*
 * Local variables:
 * c-indentation-style: bsd
 * c-basic-offset: 4
 * indent-tabs-mode: nil
 * End:
 *
 * vim: expandtab shiftwidth=4:
*/