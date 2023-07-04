#include "vm.h"
#include <stdio.h>
#include <stdint.h>

/* prints 1 byte as binary (so 0xf0 becomes "11110000") */
void
print_byte_as_binary (unsigned char byte)
{
    for (char j = 7; j >= 0; j--) {
        printf ("%d", (byte >> j) & 1);
    }
}

void
print_binary (void *data, size_t bytes)
{
    unsigned char byte; /* current byte */

    for (size_t i = 0; i < bytes; i++)
    {
        if (i > 0)
        {
            printf(" ");
        }

        byte = *((unsigned char *) data + i);
        print_byte_as_binary (byte);
    }
}

void
vm_print (struct VM *vm)
{
    printf ("ic: ");
    print_binary (&vm->program_counter, sizeof (vm->program_counter));
    printf (" 0x%x %d\n", vm->program_counter, vm->program_counter);

    printf ("ra: ");
    print_binary (&vm->register_a, sizeof (vm->register_a));
    printf (" 0x%x %d\n", vm->register_a, vm->register_a);

    printf ("rb: ");
    print_binary (&vm->register_b, sizeof (vm->register_b));
    printf (" 0x%x %d\n", vm->register_b, vm->register_b);
}

int
vm_read_byte (byte_t *result, struct VM *vm)
{
    /* detect overflow */
    if (vm->program_counter >= WORD_T_MAX)
    {
        return 0;
    }

    *result = vm->memory[vm->program_counter];
    vm->program_counter++;

    return 1;
}

int
vm_read_word (word_t *result, struct VM *vm)
{
    /* get bytes (little endian) */
    for (int i = 0; i < sizeof (*result); i++)
    {
        byte_t byte;
        if (!vm_read_byte (&byte, vm))
        {
            return 0; /* program counter overflow */
        }
        word_t shifted = (word_t) byte;
        shifted <<= i * 8;
        *result |= shifted;
    }

    return 1;
}

int
vm_op_lda (struct VM *vm)
{
    /* get address (little endian) */
    word_t address = 0;
    for (int i = 0; i < sizeof (address); i++)
    {
        byte_t byte;
        if (!vm_read_byte (&byte, vm))
        {
            return 0; /* program counter overflow */
        }

        word_t shifted = (word_t) byte;
        shifted <<= i * 8;
        address |= shifted;
    }

    /* load data */
    vm->register_a = vm->memory[address];

    return 1;
}

int
vm_op_add (struct VM *vm)
{
    /* detect overflow */
    word_t room = WORD_T_MAX - vm->register_a;
    if (vm->register_b > room)
    {
        return 0;
    }

    word_t result = vm->register_a + vm->register_b;
    vm->register_a = result;

    return 1;
}

int
vm_op_sub (struct VM *vm)
{
    /* detect overflow */
    if (vm->register_a < vm->register_b)
    {
        return 0;
    }

    word_t result = vm->register_a - vm->register_b;
    vm->register_a = result;

    return 1;
}