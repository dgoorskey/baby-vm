#include <stdio.h>
#include <assert.h>
#include "../vm.h"

int
main (void)
{
    assert (word_t_add(1, 1) == 2);
    return 0;
}
