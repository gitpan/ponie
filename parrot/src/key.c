/*
Copyright: 2001-2003 The Perl Foundation.  All Rights Reserved.
$Id: key.c,v 1.45 2004/01/26 23:16:05 mikescott Exp $

=head1 NAME

src/key.c - Base vtable calling functions

=head1 DESCRIPTION

The base vtable calling functions.

=head2 Functions

=over 4

=cut

*/

#include "parrot/parrot.h"

/*

=item C<PMC *
key_new(struct Parrot_Interp *interpreter)>

Returns a new C<Key> PMC.

=cut

*/

PMC *
key_new(struct Parrot_Interp *interpreter)
{
    PMC *key = pmc_new(interpreter, enum_class_Key);

    return key;
}

/*

=item C<PMC *
key_new_integer(struct Parrot_Interp *interpreter, INTVAL value)>

Returns a new integer C<Key> PMC with value C<value>.

=cut

*/

PMC *
key_new_integer(struct Parrot_Interp *interpreter, INTVAL value)
{
    PMC *key = pmc_new(interpreter, enum_class_Key);

    PObj_get_FLAGS(key) |= KEY_integer_FLAG;
    key->cache.int_val = value;

    return key;
}

/*

=item C<PMC *
key_new_number(struct Parrot_Interp *interpreter, FLOATVAL value)>

Returns a new number C<Key> PMC with value C<value>.

=cut

*/

PMC *
key_new_number(struct Parrot_Interp *interpreter, FLOATVAL value)
{
    PMC *key = pmc_new(interpreter, enum_class_Key);

    PObj_get_FLAGS(key) |= KEY_number_FLAG;
    key->cache.num_val = value;

    return key;
}

/*

=item C<PMC *
key_new_string(struct Parrot_Interp *interpreter, STRING *value)>

Returns a new string C<Key> PMC with value C<value>.

=cut

*/

PMC *
key_new_string(struct Parrot_Interp *interpreter, STRING *value)
{
    PMC *key = pmc_new(interpreter, enum_class_Key);

    PObj_get_FLAGS(key) |= KEY_string_FLAG;
    key->cache.string_val = value;

    return key;
}

/*

=item C<PMC *
key_new_cstring(struct Parrot_Interp *interpreter, const char *value)>

Returns a new string C<Key> PMC with value C<value> converted to a
C<STRING>.

=cut

*/

PMC *
key_new_cstring(struct Parrot_Interp *interpreter, const char *value)
{
    return key_new_string(interpreter,
            string_from_cstring(interpreter, value, 0));
}

/*

=item C<PMC *
key_new_pmc(struct Parrot_Interp *interpreter, PMC *value)>

Returns a new PMC C<Key> PMC with value C<value>.

=cut

*/

PMC *
key_new_pmc(struct Parrot_Interp *interpreter, PMC *value)
{
    PMC *key = pmc_new(interpreter, enum_class_Key);

    PObj_get_FLAGS(key) |= KEY_pmc_FLAG;
    key->cache.pmc_val = value;

    return key;
}

/*

=item C<void
key_set_integer(struct Parrot_Interp *interpreter, PMC *key, INTVAL value)>

Set the integer C<value> in C<key>.

=cut

*/

void
key_set_integer(struct Parrot_Interp *interpreter, PMC *key, INTVAL value)
{
    PObj_get_FLAGS(key) &= ~KEY_type_FLAGS;
    PObj_get_FLAGS(key) |= KEY_integer_FLAG;
    key->cache.int_val = value;

    return;
}

/*

=item C<void
key_set_register(struct Parrot_Interp *interpreter, PMC *key, INTVAL value,
                 INTVAL flag)>

Set the register C<value> in C<key>.

=cut

*/

void
key_set_register(struct Parrot_Interp *interpreter, PMC *key, INTVAL value,
                 INTVAL flag)
{
    PObj_get_FLAGS(key) &= ~KEY_type_FLAGS;
    PObj_get_FLAGS(key) |= KEY_register_FLAG | flag;
    key->cache.int_val = value;

    return;
}

/*

=item C<void
key_set_number(struct Parrot_Interp *interpreter, PMC *key, FLOATVAL value)>

Set the number C<value> in C<key>.

=cut

*/

void
key_set_number(struct Parrot_Interp *interpreter, PMC *key, FLOATVAL value)
{
    PObj_get_FLAGS(key) &= ~KEY_type_FLAGS;
    PObj_get_FLAGS(key) |= KEY_number_FLAG;
    key->cache.num_val = value;

    return;
}

/*

=item C<void
key_set_string(struct Parrot_Interp *interpreter, PMC *key, STRING *value)>

Set the string C<value> in C<key>.

=cut

*/

void
key_set_string(struct Parrot_Interp *interpreter, PMC *key, STRING *value)
{
    PObj_get_FLAGS(key) &= ~KEY_type_FLAGS;
    PObj_get_FLAGS(key) |= KEY_string_FLAG;
    key->cache.string_val = value;

    return;
}

/*

=item C<void
key_set_pmc(struct Parrot_Interp *interpreter, PMC *key, PMC *value)>

Set the PMC C<value> in C<key>.

=cut

*/

void
key_set_pmc(struct Parrot_Interp *interpreter, PMC *key, PMC *value)
{
    PObj_get_FLAGS(key) &= ~KEY_type_FLAGS;
    PObj_get_FLAGS(key) |= KEY_pmc_FLAG;
    key->cache.pmc_val = value;

    return;
}

/*

=item C<INTVAL
key_type(struct Parrot_Interp *interpreter, PMC *key)>

Returns the type of C<key>.

=cut

*/

INTVAL
key_type(struct Parrot_Interp *interpreter, PMC *key)
{
    return (PObj_get_FLAGS(key) & KEY_type_FLAGS) & ~KEY_register_FLAG;
}

/*

=item C<INTVAL
key_integer(struct Parrot_Interp *interpreter, PMC *key)>

=cut

*/

INTVAL
key_integer(struct Parrot_Interp *interpreter, PMC *key)
{
    PMC *reg;

    switch (PObj_get_FLAGS(key) & KEY_type_FLAGS) {
    case KEY_integer_FLAG:
        return key->cache.int_val;
    case KEY_integer_FLAG | KEY_register_FLAG:
        return interpreter->int_reg.registers[key->cache.int_val];
    case KEY_pmc_FLAG:
        return VTABLE_get_integer(interpreter,
                                                       key->cache.pmc_val);
    case KEY_pmc_FLAG | KEY_register_FLAG:
        reg = interpreter->pmc_reg.registers[key->cache.int_val];
        return VTABLE_get_integer(interpreter, reg);
    default:
        internal_exception(INVALID_OPERATION, "Key not an integer!\n");
        return 0;
    }
}

/*

=item C<FLOATVAL
key_number(struct Parrot_Interp *interpreter, PMC *key)>

=cut

*/

FLOATVAL
key_number(struct Parrot_Interp *interpreter, PMC *key)
{
    PMC *reg;

    switch (PObj_get_FLAGS(key) & KEY_type_FLAGS) {
    case KEY_number_FLAG:
        return key->cache.num_val;
    case KEY_number_FLAG | KEY_register_FLAG:
        return interpreter->num_reg.registers[key->cache.int_val];
    case KEY_pmc_FLAG:
        return VTABLE_get_number(interpreter,
                                                      key->cache.pmc_val);
    case KEY_pmc_FLAG | KEY_register_FLAG:
        reg = interpreter->pmc_reg.registers[key->cache.int_val];
        return VTABLE_get_number(interpreter, reg);
    default:
        internal_exception(INVALID_OPERATION, "Key not a number!\n");
        return 0;
    }
}

/*

=item C<STRING *
key_string(struct Parrot_Interp *interpreter, PMC *key)>

=cut

*/

STRING *
key_string(struct Parrot_Interp *interpreter, PMC *key)
{
    PMC *reg;

    switch (PObj_get_FLAGS(key) & KEY_type_FLAGS) {
    case KEY_string_FLAG:
        return key->cache.string_val;
    case KEY_string_FLAG | KEY_register_FLAG:
        return interpreter->string_reg.registers[key->cache.int_val];
    case KEY_pmc_FLAG:
        return VTABLE_get_string(interpreter,
                                                      key->cache.pmc_val);
    case KEY_pmc_FLAG | KEY_register_FLAG:
        reg = interpreter->pmc_reg.registers[key->cache.int_val];
        return VTABLE_get_string(interpreter, reg);
    default:
        internal_exception(INVALID_OPERATION, "Key not a string!\n");
        return 0;
    }
}

/*

=item C<PMC *
key_pmc(struct Parrot_Interp *interpreter, PMC *key)>

These functions return the integer/number/string/PMC values of C<key> if
possible. Otherwise they throws an exceptions.

=cut

*/

PMC *
key_pmc(struct Parrot_Interp *interpreter, PMC *key)
{
    switch (PObj_get_FLAGS(key) & KEY_type_FLAGS) {
    case KEY_pmc_FLAG:
        return key->cache.pmc_val;
    case KEY_pmc_FLAG | KEY_register_FLAG:
        return interpreter->pmc_reg.registers[key->cache.int_val];
    default:
        internal_exception(INVALID_OPERATION, "Key not a PMC!\n");
        return 0;
    }
}

/*

=item C<PMC *
key_next(struct Parrot_Interp *interpreter, PMC *key)>

Returns the next key if C<key> is in a sequence of linked keys.

=cut

*/

PMC *
key_next(struct Parrot_Interp *interpreter, PMC *key)
{
    return PMC_data(key);
}

/*

=item C<PMC *
key_append(struct Parrot_Interp *interpreter, PMC *key1, PMC *key2)>

Appends C<key2> to C<key1>.

Note that if C<key1> is not the last key in a sequence linked keys then
the last key will be found and C<key2> appended to that.

Returns C<key1>.

=cut

*/

PMC *
key_append(struct Parrot_Interp *interpreter, PMC *key1, PMC *key2)
{
    PMC *tail = key1;

    while (PMC_data(tail)) {
        tail = PMC_data(tail);
    }

    PMC_data(tail) = key2;

    return key1;
}

/*

=item C<void
key_mark(struct Parrot_Interp *interpreter, PMC *key)>

Marks C<key> as live.

=cut

*/

void
key_mark(struct Parrot_Interp *interpreter, PMC *key)
{
    pobject_lives(interpreter, (PObj *) key);
    if ( ((PObj_get_FLAGS(key) & KEY_type_FLAGS) == KEY_string_FLAG) ||
       ((PObj_get_FLAGS(key) & KEY_type_FLAGS) == KEY_pmc_FLAG) )
        pobject_lives(interpreter, (PObj *)key->cache.string_val);
    if ((PObj_get_FLAGS(key) & KEY_type_FLAGS) == KEY_integer_FLAG)
        return;

    if (PMC_data(key))
        pobject_lives(interpreter, (PObj *)PMC_data(key));

}

/*

=back

=head1 SEE ALSO

F<include/parrot/key.h>.

=head1 HISTORY

Initial version by Jeff G. on 2001.12.05.

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
