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
        ArrayPtr.clear(self)
    }

    mtd opPostCopy()
    {
        loop i: count
        {
            let newPtr = Memory.new'T(allocator)
            dref newPtr = dref buffer[i]
            buffer[i]   = newPtr
        }
    }

    // Release content
    mtd clear()
    {
        visit item: dref self do
            Memory.delete(item, allocator)
        base.count = 0
    }

    // Delete one pointer allocated here
    mtd deletePtr(item: *T)
    {
        Memory.delete(item, allocator)
    }

    // Allocate a new pointer by using the contextual allocator
    mtd newPtr()->*T
    {
        return Memory.new'T(allocator)
    }

    // Allocate a new pointer, and add it to the array
    mtd addNewPtr()->*T
    {
        let ptr = newPtr()
        add(ptr)
        return ptr
    }
}
