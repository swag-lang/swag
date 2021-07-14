
using Core

//#[Swag.PrintBc]
#test
{
    ptr0 := Memory.alloc(10, null, 1)
    Memory.free(ptr0, 10)

    ptr1 := Memory.alloc(10, null, 2)
    Memory.free(ptr1, 10)

    ptr2 := Memory.alloc(10, null, 4)
    Memory.free(ptr2, 10)

    ptr3 := Memory.alloc(10, null, 8)
    Memory.free(ptr3, 10)

    ptr4 := Memory.alloc(10, null, 16)
    Memory.free(ptr4, 10)
}
