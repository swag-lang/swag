#global export
using Swag

struct(T) ArrayPtr
{
    using base: Array'(*T)
}

impl ArrayPtr
{
    mtd opDrop()
    {
        clear()
    }

    mtd clear()
    {
        visit item: dref self
            Memory.delete(item, allocator)
        base.count = 0
    }

    // Allocate a new pointer by using the contextual allocator
    mtd newPtr()->*T
    {
        return Memory.new'T(allocator)
    }

    // Allocate a new pointer, and add it to the array
    mtd addNewPtr()->*T
    {
        ptr := newPtr()
        base.add(ptr)
        return ptr
    }
}