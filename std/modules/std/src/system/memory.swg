public namespace std.memory;
using swag

// Copy one block of memory to a given address
func copy(dest: *void, src: const *void, size: u32)
{
    if size @memcpy(dest, src, size)
}

// Compare two blocks of memory
func compare(dest, src: const *void, size: u32)->s32
{
    if !size return 0
    return @memcmp(dest, src, size)
}

// Clear one block of memory
func clear(dest: *void, size: u32)
{
    if size @memset(dest, 0, size)
}

// Set the content of one block of memory to a specific byte value
func set(dest: *void, value: u8, size: u32)
{
    if size @memset(dest, value, size)
}

// Free the specified memory block
func free(buffer: *void, allocator: IAllocator)
{
    if !buffer
        return
    debug.assert(allocator != null)
    req := AllocatorRequest{mode: AllocatorMode.Free, address: buffer}
    allocator.alloc(&req)
}

// Reallocate/allocate a given amount of bytes
func realloc(buffer: *void, sizeInBytes: u32, allocator: IAllocator)->*void
{
    debug.assert(allocator != null)
    req := AllocatorRequest{mode: AllocatorMode.Realloc, size: sizeInBytes, address: buffer}
    allocator.alloc(&req)
    return req.address
}

// Allocate a given amount of bytes
func alloc(sizeInBytes: u32, allocator: IAllocator)->*void
{
    debug.assert(allocator != null)
    req := AllocatorRequest{mode: AllocatorMode.Alloc, size: sizeInBytes}
    allocator.alloc(&req)
    return req.address
}
