#import "core"
using core

//#[swag.printbc]
#test
{
    ptr0 := memory.alloc(10, null, 1)
    memory.free(ptr0, 10)

    ptr1 := memory.alloc(10, null, 2)
    memory.free(ptr1, 10)

    ptr2 := memory.alloc(10, null, 4)
    memory.free(ptr2, 10)

    ptr3 := memory.alloc(10, null, 8)
    memory.free(ptr3, 10)

    ptr4 := memory.alloc(10, null, 16)
    memory.free(ptr4, 10)
}
