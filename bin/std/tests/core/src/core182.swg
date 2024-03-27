using Core

//#[Swag.PrintBc]
#test
{
    let ptr0 = Memory.alloc(10, null, 1)
    Memory.free(ptr0, 10)

    let ptr1 = Memory.alloc(10, null, 2)
    @assert(cast(u64) ptr1 & 0x1 == 0)
    Memory.free(ptr1, 10)

    let ptr2 = Memory.alloc(10, null, 4)
    @assert(cast(u64) ptr2 & 0x3 == 0)
    Memory.free(ptr2, 10)

    let ptr3 = Memory.alloc(10, null, 8)
    @assert(cast(u64) ptr3 & 0x7 == 0)
    Memory.free(ptr3, 10)

    let ptr4 = Memory.alloc(10, null, 16)
    @assert(cast(u64) ptr4 & 0xF == 0)
    Memory.free(ptr4, 10)
}
