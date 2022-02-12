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
        visit item: dref self
            Memory.delete(item, allocator)
    }

    // Allocate a new pointer by using the contextual allocator
    mtd newPtr()->*T
    {
        return Memory.new'T(allocator)
    }
}