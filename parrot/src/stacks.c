/*
Copyright: 2001-2003 The Perl Foundation.  All Rights Reserved.
$Id: stacks.c,v 1.71 2004/03/04 11:16:02 mikescott Exp $

=head1 NAME

src/stacks.c - Stack handling routines for Parrot

=head1 DESCRIPTION

The stack is stored as a doubly-linked list of chunks (C<Stack_Chunk>),
where each chunk has room for C<STACK_CHUNK_DEPTH> entries. The
invariant maintained is that there is always room for another entry; if
a chunk is filled, a new chunk is added onto the list before returning.

Each chunk contains a C<Buffer> which contains, via C<obj.u>, a void
pointer C<bufstart> to the start of the actual memory.

We use "tree-stacks" and COW (copy-on-write) semantics in order to make
continuations easy. A stack chunk is a bufferlike structure and may be
GCed or COWed. As top chunks are COWed on usage, its only safe to walk
the stack from top down via the prev pointers.
C<< stack_chunk->prev->next >> may not equal C<stack_chunk> if C<prev> is
COWed and not copied yet.

COWed chunks are NOT magically copied, so any attempts to write should
take care that the chunk may be shared. "Write" in this case is not only
limited to pushing an item on. ANY change to the stack state, including
a pop, is a "write". Think "write" in terms of changing any execution
context, not values.

Note, however, that API calls C<stack_push>, C<stack_pop> and
C<rotate_entries> will take care of COW semantics themselves.

=head2 Functions

=over 4

=cut

*/

#include "parrot/parrot.h"
#include <assert.h>

/*

=item C<void stack_system_init(Interp *interpreter)>

Called from C<make_interpreter()> to initialize the interpreter's
register stacks.

=cut

*/

void stack_system_init(Interp *interpreter)
{
    make_bufferlike_pool(interpreter, sizeof(Stack_Chunk_t));
}

/*

=item C<Stack_Chunk_t *
new_stack(Interp *interpreter, const char *name)>

Create a new stack and name it. C<< stack->name >> is used for
debugging/error reporting.

=cut

*/

Stack_Chunk_t *
new_stack(Interp *interpreter, const char *name)
{

    return cst_new_stack(interpreter, name,
            sizeof(Stack_Entry_t), STACK_CHUNK_DEPTH);
}


/*

=item C<void
mark_stack(struct Parrot_Interp *interpreter,
           Stack_Chunk_t *chunk)>

Mark entries in a stack structure during GC.

=cut

*/

void
mark_stack(struct Parrot_Interp *interpreter,
           Stack_Chunk_t *chunk)
{
    Stack_Entry_t *entry;
    size_t i;

    for (; chunk && chunk->prev; chunk = chunk->prev)
        ;
    for (; chunk; chunk = chunk->next) {

        pobject_lives(interpreter, (PObj *)chunk);
        entry = (Stack_Entry_t *)(chunk->bufstart);
        for (i = 0; i < chunk->used; i++) {
            switch (entry[i].entry_type) {
                case STACK_ENTRY_PMC:
                    if (entry[i].entry.pmc_val) {
                        pobject_lives(interpreter,
                                      (PObj *)entry[i].entry.pmc_val);
                    }
                    break;
                case STACK_ENTRY_STRING:
                    if (entry[i].entry.string_val) {
                        pobject_lives(interpreter,
                                      (PObj *)entry[i].entry.string_val);
                    }
                    break;
                default:
                    break;
            }
        }
    }
}
/*

=item C<void
stack_destroy(Stack_Chunk_t * top)>

GC does it all.

=cut

*/

void
stack_destroy(Stack_Chunk_t * top)
{
   /* GC does it all */
}

/*

=item C<size_t
stack_height(Interp *interpreter, Stack_Chunk_t *top)>

Returns the height of the stack. The maximum "depth" is height - 1.

=cut

*/

size_t
stack_height(Interp *interpreter, Stack_Chunk_t *top)
{
    Stack_Chunk_t *chunk;
    size_t height = top->used;

    for (chunk = top->prev; chunk; chunk = chunk->prev)
        height += chunk->used;
    assert(height == (top->n_chunks - 1) * STACK_CHUNK_DEPTH +
            top->used);

    return height;
}


/*

=item C<Stack_Entry_t *
stack_entry(Interp *interpreter, Stack_Chunk_t *stack, Intval depth)>

If C<< depth >= 0 >>, return the entry at that depth from the top of the
stack, with 0 being the top entry. If C<depth < 0>, then return the
entry C<|depth|> entries from the bottom of the stack. Returns C<NULL>
if C<|depth| > number> of entries in stack.

=cut

*/

Stack_Entry_t *
stack_entry(Interp *interpreter, Stack_Chunk_t *stack, Intval depth)
{
    Stack_Chunk_t *chunk;
    Stack_Entry_t *entry = NULL;
    size_t offset = (size_t)depth;

    /* For negative depths, look from the bottom of the stack up. */
    if (depth < 0) {
        depth = stack_height(interpreter, interpreter->ctx.user_stack) + depth;
        if (depth < 0)
            return NULL;
        offset = (size_t)depth;
    }
    chunk = stack;          /* Start at top */
    while (chunk != NULL && offset >= chunk->used) {
        offset -= chunk->used;
        chunk = chunk->prev;
    }
    if (chunk == NULL)
        return NULL;
    if (offset < chunk->used) {
        entry = (Stack_Entry_t *)PObj_bufstart(chunk) +
            chunk->used - offset - 1;
    }
    return entry;
}

/*

=item C<void
rotate_entries(Interp *interpreter, Stack_Chunk_t **stack_p,
               Intval num_entries)>

Rotate the top N entries by one.  If C<N > 0>, the rotation is bubble
up, so the top most element becomes the Nth element.  If C<N < 0>, the
rotation is bubble down, so that the Nth element becomes the top most
element.

=cut

*/

void
rotate_entries(Interp *interpreter, Stack_Chunk_t **stack_p, Intval num_entries)
{
    Stack_Chunk_t *stack = *stack_p;
    Stack_Entry_t temp;
    Intval i;
    Intval depth = num_entries - 1;

    if (num_entries >= -1 && num_entries <= 1) {
        return;
    }

    /* If stack is copy-on-write, copy it before we can execute on it */
    if (PObj_COW_TEST( (Buffer *) stack)) {
        stack_unmake_COW(interpreter, stack);
        if (depth >= STACK_CHUNK_DEPTH)
            internal_exception(1, "Unhandled deep rotate for COWed stack");
    }


    if (num_entries < 0) {
        num_entries = -num_entries;
        depth = num_entries - 1;

        if (stack_height(interpreter, stack) < (size_t)num_entries) {
            internal_exception(ERROR_STACK_SHALLOW, "Stack too shallow!\n");
        }

        temp = *stack_entry(interpreter, stack, depth);
        for (i = depth; i > 0; i--) {
            *stack_entry(interpreter, stack, i) =
                *stack_entry(interpreter, stack, i - 1);
        }

        *stack_entry(interpreter, stack, 0) = temp;
    }
    else {

        if (stack_height(interpreter, stack) < (size_t)num_entries) {
            internal_exception(ERROR_STACK_SHALLOW, "Stack too shallow!\n");
        }
        temp = *stack_entry(interpreter, stack, 0);
        for (i = 0; i < depth; i++) {
            *stack_entry(interpreter, stack, i) =
                *stack_entry(interpreter, stack, i + 1);
        }

        *stack_entry(interpreter, stack, depth) = temp;
    }
}

/*

=item C<void
stack_push(Interp *interpreter, Stack_Chunk_t **stack_p,
           void *thing, Stack_entry_type type, Stack_cleanup_method cleanup)>

Push something on the generic stack.

Note that the cleanup pointer, if non-C<NULL>, points to a routine
that'll be called when the entry is removed from the stack. This is
handy for those cases where you need some sort of activity to take place
when an entry is removed, such as when you push a lexical lock onto the
call stack, or localize (or tempify, or whatever we're calling it)
variable or something.

=cut

*/

void
stack_push(Interp *interpreter, Stack_Chunk_t **stack_p,
           void *thing, Stack_entry_type type, Stack_cleanup_method cleanup)
{
    Stack_Entry_t *entry = stack_prepare_push(interpreter, stack_p);

    /* Remember the type */
    entry->entry_type = type;
    /* Remember the cleanup function */
    entry->cleanup = cleanup;
    /* Store our thing */
    switch (type) {
        case STACK_ENTRY_INT:
            entry->entry.int_val = *(Intval *)thing;
            break;
        case STACK_ENTRY_FLOAT:
            entry->entry.num_val = *(Floatval *)thing;
            break;
        case STACK_ENTRY_PMC:
            entry->entry.pmc_val = (PMC *)thing;
            break;
        case STACK_ENTRY_STRING:
            entry->entry.string_val = (String *)thing;
            break;
        case STACK_ENTRY_POINTER:
        case STACK_ENTRY_DESTINATION:
        case STACK_ENTRY_CORO_MARK:
            entry->entry.struct_val = thing;
            break;
        default:
            internal_exception(ERROR_BAD_STACK_TYPE,
                    "Invalid Stack_Entry_type!\n");
            break;
    }
}

/*

=item C<void *
stack_pop(Interp *interpreter, Stack_Chunk_t **stack_p,
          void *where, Stack_entry_type type)>

Pop off an entry and return a pointer to the contents.

=cut

*/

void *
stack_pop(Interp *interpreter, Stack_Chunk_t **stack_p,
          void *where, Stack_entry_type type)
{
    Stack_Entry_t *entry = stack_prepare_pop(interpreter, stack_p);

    /* Types of 0 mean we don't care */
    if (type && entry->entry_type != type) {
        internal_exception(ERROR_BAD_STACK_TYPE,
                           "Wrong type on top of stack!\n");
    }

    /* Cleanup routine? */
    if (type != STACK_ENTRY_CORO_MARK && entry->cleanup) {
        (*entry->cleanup) (entry);
    }

    /* Sometimes the caller doesn't care what the value was */
    if (where == NULL) {
        return NULL;
    }

    /* Snag the value */
    switch (type) {
    case STACK_ENTRY_INT:
        *(Intval *)where = entry->entry.int_val;
        break;
    case STACK_ENTRY_FLOAT:
        *(Floatval *)where = entry->entry.num_val;
        break;
    case STACK_ENTRY_PMC:
        *(PMC **)where = entry->entry.pmc_val;
        break;
    case STACK_ENTRY_STRING:
        *(String **)where = entry->entry.string_val;
        break;
    case STACK_ENTRY_POINTER:
    case STACK_ENTRY_DESTINATION:
    case STACK_ENTRY_CORO_MARK:
        *(void **)where = entry->entry.struct_val;
        break;
    default:
        internal_exception(ERROR_BAD_STACK_TYPE,
                           "Wrong type on top of stack!\n");
        break;
    }

    return where;
}

/*

=item C<void *
pop_dest(Interp *interpreter)>

Pop off a destination entry and return a pointer to the contents.

=cut

*/

void *
pop_dest(Interp *interpreter)
{
    /* We don't mind the extra call, so we do this: (previous comment
     * said we *do* mind, but I say let the compiler decide) */
    void *dest;
    (void)stack_pop(interpreter, &interpreter->ctx.control_stack,
                    &dest, STACK_ENTRY_DESTINATION);
    return dest;
}

/*

=item C<void *
stack_peek(Interp *interpreter, Stack_Chunk_t *stack_base,
           Stack_entry_type *type)>

Peek at stack and return pointer to entry and the type of the entry.

=cut

*/

void *
stack_peek(Interp *interpreter, Stack_Chunk_t *stack_base,
           Stack_entry_type *type)
{
    Stack_Entry_t *entry = stack_entry(interpreter, stack_base, 0);
    if (entry == NULL) {
        return NULL;
    }

    if (type != NULL) {
        *type = entry->entry_type;
    }
    switch (entry->entry_type) {
        case STACK_ENTRY_POINTER:
        case STACK_ENTRY_DESTINATION:
        case STACK_ENTRY_CORO_MARK:
            return entry->entry.struct_val;
        default:
            return (void *)entry->entry.pmc_val;
    }
}

/*

=item C<Stack_entry_type
get_entry_type(Interp *interpreter, Stack_Entry_t *entry)>

Returns the stack entry type of C<entry>.

=cut

*/

Stack_entry_type
get_entry_type(Interp *interpreter, Stack_Entry_t *entry)
{
    return entry->entry_type;
}

/*

=back

=head1 SEE ALSO

F<include/parrot/stacks.h> and F<include/parrot/enums.h>.

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