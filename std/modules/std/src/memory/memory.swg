public namespace memory;
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

// Align a value to the given amount
func align(value: u32, alignement: u32)->u32
{
    toAlign := alignement ?? @sizeof(*void)
    result := value & ~(toAlign - 1)
    if result < value result += toAlign
    return result
}

// Align a pointer to the given amount
func align(value: const *void, alignement: u32)->const *void
{
    toAlign := alignement ?? @sizeof(*void)
    res64 := cast(u64) value
    result := res64 & ~(toAlign - 1)
    if result < res64 result += toAlign
    return cast(const *void) result
}