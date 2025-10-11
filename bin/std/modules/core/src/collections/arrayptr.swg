#global export
using Swag

// A specialized dynamic array for holding and owning pointers.
// This struct extends the base 'Array' to correctly manage the lifecycle
// (allocation and deallocation) of the pointed-to objects.
struct(T) ArrayPtr
{
    using base: Array'(*T)
}

impl ArrayPtr
{
    // Ensures that all pointed-to objects are deallocated when the array goes out of scope.
    mtd opDrop()
    {
        ArrayPtr.clear(me)
    }

    // Handles the deep copy of the array and the objects it points to.
    // When the array is copied, new memory is allocated for each element, and the
    // content is copied over, ensuring the new array owns its own set of objects.
    mtd opPostCopy()
    {
        for i in .count
        {
            let newPtr = Memory.new'T(.allocator)
            dref newPtr = dref .buffer[i]
            .buffer[i]  = newPtr
        }
    }

    // Deallocates all objects pointed to by the elements in the array and clears the array.
    // The array's count is set to 0, but the capacity is not changed.
    mtd clear()
    {
        foreach item in dref me do
            Memory.delete(item, .allocator)
        .count = 0
    }

    // Deallocates the memory for a single given pointer.
    // The pointer is expected to have been allocated using this array's allocator.
    mtd deletePtr(item: *T)
    {
        Memory.delete(item, .allocator)
    }

    // Allocates memory for a new object of type T using the array's allocator.
    // Returns a pointer to the newly allocated memory.
    mtd newPtr()->*T
    {
        return Memory.new'T(.allocator)
    }

    // Allocates a new object, adds its pointer to the end of the array, and returns the pointer.
    mtd addNewPtr()->*T
    {
        let ptr = .newPtr()
        .add(ptr)
        return ptr
    }
}
