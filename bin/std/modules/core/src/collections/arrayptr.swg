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
        @print("drop")
        visit item: dref self
            Memory.delete(item)
    }
}