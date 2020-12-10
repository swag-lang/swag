public namespace memory;
using swag

// Copy one block of memory to a given address
func copy(dest: *void, src: const *void, size: u32)
{
    if size @memcpy(dest, src, cast(uint) size)
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
    if size @memset(dest, 0, cast(uint) size)
}

// Set the content of one block of memory to a specific byte value
func set(dest: *void, value: u8, size: uint)
{
    if size @memset(dest, value, size)
}

// Align a value to the given amount
func align(value: uint, alignement: u32)->uint
{
    toAlign := alignement ?? @sizeof(*void)
    result := value & ~(toAlign - 1)
    if result < value result += toAlign
    return result
}

// Align a pointer to the given amount
func align(value: const *void, alignement: u32)->const *void
{
    return autocast align(cast(uint) value, alignement)
}