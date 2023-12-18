#global public
#global namespace Memory
using Swag

// Copy one block of memory to a given address
func copy(dest: ^void, src: const ^void, size: u64)
{
    if size @memcpy(dest, src, size)
}

// Move one block of memory to a given address
func copyOver(dest: ^void, src: const ^void, size: u64)
{
    if size @memmove(dest, src, size)
}

// Compare two blocks of memory
func compare(dest, src: const ^void, size: u64)->s32
{
    if !size return 0
    return @memcmp(dest, src, size)
}

// Compare two blocks of memory
func(T) equals(dest, src: const *T)->bool
{
    return @memcmp(dest, src, @sizeof(T)) == 0
}

// Clear one block of memory
func clear(dest: ^void, size: u64)
{
    if size @memset(dest, 0, size)
}

// Set the content of one block of memory to a specific byte value
func set(dest: ^void, value: u8, size: u64)
{
    if size @memset(dest, value, size)
}

// Align a value to the given amount
#[Swag.Overload]
func align(value: u64, alignement: u32)->u64
{
    let toAlign = (alignement orelse @sizeof(*void)) - 1
    return (value + toAlign) & ~toAlign
}

// Align a pointer to the given amount
#[Swag.Overload]
func align(value: const ^void, alignement: u32)->const ^void
{
    return acast align(cast(u64) value, alignement)
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
    var prev = @getcontext()
    defer @setcontext(prev)

    var cxt = dref prev
    cxt.allocator = allocator
    @setcontext(cxt)

    #macro
    {
        #mixin #up stmt
    }
}

// Push the temporary allocator for the given block of code
#[Swag.Macro]
func pushTempAllocator(stmt: code)
{
    var prev = @getcontext()
    defer @setcontext(prev)

    var cxt = dref prev
    cxt.allocator = @getcontext().tempAllocator
    @setcontext(cxt)

    #macro
    {
        #mixin #up stmt
    }
}