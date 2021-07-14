#global public
#global namespace Memory
using Swag

// Copy one block of memory to a given address
func copy(dest: *void, src: const *void, size: uint)
{
    if size @memcpy(dest, src, size)
}

// Move one block of memory to a given address
func copyOver(dest: *void, src: const *void, size: uint)
{
    if size @memmove(dest, src, size)
}

// Compare two blocks of memory
func compare(dest, src: const *void, size: uint)->s32
{
    if !size return 0
    return @memcmp(dest, src, size)
}

// Clear one block of memory
func clear(dest: *void, size: uint)
{
    if size @memset(dest, 0, size)
}

// Set the content of one block of memory to a specific byte value
func set(dest: *void, value: u8, size: uint)
{
    if size @memset(dest, value, size)
}

// Align a value to the given amount
func align(value: uint, alignement: u32)->uint
{
    toAlign := alignement orelse @sizeof(*void)
    result := value & ~(toAlign - 1)
    if result < value result += toAlign
    return result
}

// Align a pointer to the given amount
func align(value: const *void, alignement: u32)->const *void
{
    return acast align(cast(uint) value, alignement)
}

// Clear the temporary allocator
func freeTemp()
{
    @getcontext().tempAllocator.freeAll()
}

// Push a new allocator for the given block of code
#[Swag.Macro]
func pushAllocator(allocator: Swag.IAllocator, stmt: code)
{
    prev := @getcontext()
    defer @setcontext(prev)

    cxt := dref prev
    cxt.allocator = allocator
    @setcontext(cxt)

    #macro
    {
        #mixin `stmt
    }
}

// Push the temporary allocator for the given block of code
#[Swag.Macro]
func pushTempAllocator(stmt: code)
{
    prev := @getcontext()
    defer @setcontext(prev)

    cxt := dref prev
    cxt.allocator = @getcontext().tempAllocator
    @setcontext(cxt)

    #macro
    {
        #mixin `stmt
    }
}