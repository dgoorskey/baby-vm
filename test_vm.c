#include <stdio.h>
#include <assert.h>
#include "vm.h"

void
test_vm_read_byte (byte_t *initial_memory, size_t initial_memory_size,
                   word_t initial_program_counter, byte_t expected,
                   int should_overflow)
{
    struct VM vm;
    for (int i = 0; i < initial_memory_size; i++)
    {
        vm.memory[i] = initial_memory[i];
    }
    vm.program_counter = initial_program_counter;

    byte_t result;

    if (should_overflow)
    {
        assert (!vm_read_byte (&result, &vm));
    }
    else
    {
        assert (vm_read_byte (&result, &vm));
        assert (result == expected);
        assert (vm.program_counter == initial_program_counter + 1);
    }
}

void
test_vm_read_word (byte_t *initial_memory, size_t initial_memory_size,
                   word_t initial_program_counter, word_t expected,
                   int should_overflow)
{
    struct VM vm;
    for (int i = 0; i < initial_memory_size; i++)
    {
        vm.memory[i] = initial_memory[i];
    }
    vm.program_counter = initial_program_counter;

    word_t result;

    if (should_overflow)
    {
        assert (!vm_read_word (&result, &vm));
    }
    else
    {
        assert (vm_read_word (&result, &vm));
        assert (result == expected);
        assert (vm.program_counter == initial_program_counter + 2);
    }
}

void
test_vm_op_lda (byte_t *initial_memory, size_t initial_memory_size,
                word_t initial_program_counter, byte_t expected,
                int should_fail)
{
    struct VM vm;
    for (int i = 0; i < initial_memory_size; i++)
    {
        vm.memory[i] = initial_memory[i];
    }
    vm.program_counter = initial_program_counter;

    if (should_fail)
    {
        assert (!vm_op_lda (&vm));
    }
    else
    {
        assert (vm_op_lda (&vm));
        assert (vm.register_a == expected);
    }
}

void
test_vm_op_add (word_t a, word_t b, word_t expected, int should_overflow)
{
    struct VM vm;
    vm.register_a = a;
    vm.register_b = b;
    if (should_overflow)
    {
        assert (!vm_op_add (&vm));
    }
    else
    {
        assert (vm_op_add (&vm));
        assert (vm.register_a == expected);
    }
}

void
test_vm_op_sub (word_t a, word_t b, word_t expected, int should_overflow)
{
    struct VM vm;
    vm.register_a = a;
    vm.register_b = b;
    if (should_overflow)
    {
        assert (!vm_op_sub (&vm));
    }
    else
    {
        assert (vm_op_sub (&vm));
        assert (vm.register_a == expected);
    }
}

int
main (void)
{
    {
        byte_t m1[2] = {0x00u, 0x01u};
        test_vm_read_byte (m1, 2, 0, 0x00u, 0);
        byte_t m2[2] = {0x00u, 0x01u};
        test_vm_read_byte (m2, 2, 1, 0x01u, 0);
        byte_t m4[VM_MEMORY_SIZE];
        m4[WORD_T_MAX - 1] = 0x01u;
        test_vm_read_byte (m4, VM_MEMORY_SIZE, WORD_T_MAX - 1, 0x01u, 0);
        byte_t m5[VM_MEMORY_SIZE];
        test_vm_read_byte (m5, VM_MEMORY_SIZE, WORD_T_MAX, 0x00u, 1);
    }

    {
        byte_t m1[2] = {0xcdu, 0xabu};
        test_vm_read_word (m1, 2, 0, 0xabcdu, 0);
        byte_t m2[5] = {0x00u, 0x00u, 0x00u, 0xcdu, 0xabu};
        test_vm_read_word(m2, 5, 3, 0xabcdu, 0);
        byte_t m4[VM_MEMORY_SIZE];
        m4[WORD_T_MAX - 2] = 0xcdu;
        m4[WORD_T_MAX - 1] = 0xabu;
        test_vm_read_word(m4, VM_MEMORY_SIZE, WORD_T_MAX - 2, 0xabcdu, 0);
        byte_t m5[VM_MEMORY_SIZE];
        test_vm_read_word(m5, VM_MEMORY_SIZE, WORD_T_MAX, 0x0000u, 1);
    }

    {
        byte_t m1[3] = {0x02u, 0x00u, 0xffu};
        test_vm_op_lda (m1, 3, 0, 0xffu, 0);
        byte_t m2[5] = {0x00u, 0xffu, 0x00u, 0x01u, 0x00u};
        test_vm_op_lda (m2, 5, 3, 0xffu, 0);
        byte_t m3[2] = {0x00u, 0x00u};
        test_vm_op_lda (m3, 2, 0, 0x00u, 0);
        byte_t m4[VM_MEMORY_SIZE];
        test_vm_op_lda (m4, VM_MEMORY_SIZE, WORD_T_MAX, 0x00u, 1);
    }

    test_vm_op_add (0, 0, 0, 0);
    test_vm_op_add (1, 1, 2, 0);
    test_vm_op_add (WORD_T_MAX, 0, WORD_T_MAX, 0);
    test_vm_op_add (WORD_T_MAX, 1, 0, 1);
    test_vm_op_add (WORD_T_MAX, WORD_T_MAX, 0, 1);

    test_vm_op_sub (0, 0, 0, 0);
    test_vm_op_sub (1, 0, 1, 0);
    test_vm_op_sub (1, 1, 0, 0);
    test_vm_op_sub (WORD_T_MAX, 0, WORD_T_MAX, 0);
    test_vm_op_sub (WORD_T_MAX, WORD_T_MAX, 0, 0);
    test_vm_op_sub (0, WORD_T_MAX, 0, 1);

    return 0;
}