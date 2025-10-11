#global public
#global namespace Memory
using Swag

// Check if a given pointer is allocated
func assertIsAllocated(buffer: [*] void, size: u64, allocator: IAllocator = null, hint: string = null, loc = #callerlocation)
{
    if !buffer do
        return

    var req: AllocatorRequest
    req.callerLoc = loc
    req.mode      = AllocatorMode.AssertIsAllocated
    req.size      = size
    req.address   = buffer
    req.hint      = hint

    let talloc = allocator orelse @getcontext().allocator
    talloc.req(&req)
}

// Free the specified memory block
func free(buffer: [*] void, size: u64, allocator: IAllocator = null, hint: string = null, loc = #callerlocation)
{
    if !buffer do
        return

    var req: AllocatorRequest
    req.callerLoc = loc
    req.mode      = AllocatorMode.Free
    req.size      = size
    req.address   = buffer
    req.hint      = hint

    let talloc = allocator orelse @getcontext().allocator
    talloc.req(&req)
}

// Free all allocated memory of the given allocator if possible
func freeAll(allocator: IAllocator, hint: string = null, loc = #callerlocation)
{
    var req: AllocatorRequest
    req.callerLoc = loc
    req.mode      = AllocatorMode.FreeAll
    req.hint      = hint

    let talloc = allocator orelse @getcontext().allocator
    talloc.req(&req)
}

// Reallocate/allocate a given amount of bytes
func realloc(buffer: [*] void, newSize: u64, oldSize: u64, allocator: IAllocator = null, alignment: u16 = 0, hint: string = null, loc = #callerlocation)->[*] void
{
    var req: AllocatorRequest
    req.callerLoc = loc
    req.hint      = hint
    req.mode      = AllocatorMode.Realloc
    req.address   = buffer
    req.size      = newSize
    req.oldSize   = oldSize
    req.alignment = alignment

    let talloc = allocator orelse @getcontext().allocator
    talloc.req(&req)

    return req.address
}

// Allocate a given amount of bytes
func alloc(size: u64, allocator: IAllocator = null, alignment: u32 = 0, hint: string = null, loc = #callerlocation)->[*] void
{
    var req: AllocatorRequest
    req.callerLoc = loc
    req.hint      = hint
    req.mode      = AllocatorMode.Alloc
    req.size      = size
    req.alignment = alignment

    let talloc = allocator orelse @getcontext().allocator
    talloc.req(&req)

    return req.address
}

// Allocate a given amount of bytes in the temporary allocator
func tempAlloc(size: u64, alignment: u32 = 0, hint: string = null, loc = #callerlocation)->[*] void
{
    var req: AllocatorRequest
    req.callerLoc = loc
    req.hint      = hint
    req.mode      = AllocatorMode.Alloc
    req.size      = size
    req.alignment = alignment
    @getcontext().tempAllocator.IAllocator.req(&req)
    return req.address
}

// Allocate and initialize the given type
#[Swag.Overload]
func new(type: typeinfo, allocator: IAllocator = null, alignment: u32 = 0, hint: string = null, loc = #callerlocation)->*u8
{
    let userHint = hint orelse type.fullname
    let ptr      = alloc(cast(u64) type.sizeof, allocator, alignment, userHint, loc)
    if type.kind == Swag.TypeInfoKind.Struct
    {
        let typeStruct = cast(const *Swag.TypeInfoStruct) type
        typeStruct.opInit(ptr)
    }
    else do
        clear(ptr, cast(u64) type.sizeof)
    return ptr
}

// Allocate and initialize the given type
#[Swag.Overload]
func(T) new(allocator: IAllocator = null, alignment: u32 = 0, hint: string = null, loc = #callerlocation)->*T
{
    let userHint = hint orelse #stringof(T)
    let ptr      = cast(*T) alloc(#sizeof(T), allocator, alignment, userHint, loc)
    @init(ptr, 1)
    return ptr
}

// Drop and release memory of the given type
#[Swag.Overload]
func(T) delete(ptr: *T, allocator: IAllocator = null, hint: string = null, loc = #callerlocation)
{
    if !ptr do
        return
    let userHint = hint orelse #stringof(T)
    assertIsAllocated(ptr, #sizeof(T), allocator, hint, loc)
    @drop(ptr)
    free(ptr, #sizeof(T), allocator, userHint, loc)
}

// Drop and release memory of the given type
#[Swag.Overload]
func delete(ptr: *void, type: typeinfo, allocator: IAllocator = null, hint: string = null, loc = #callerlocation)
{
    if !ptr do
        return

    Debug.assert(type != null)
    let userHint = hint orelse type.name
    assertIsAllocated(ptr, cast(u64) type.sizeof, allocator, userHint, loc)

    if type.kind == TypeInfoKind.Struct
    {
        let typest = cast(const *Swag.TypeInfoStruct) type
        if typest.opDrop do
            typest.opDrop(ptr)
    }

    free(ptr, cast(u64) type.sizeof, allocator, userHint, loc)
}

// Allocate and initialize an array of the given type
#[Swag.Overload]
func(T) new(num: u64, allocator: IAllocator = null, alignment: u32 = 0, hint: string = null, loc = #callerlocation)->[*] T
{
    Debug.assert(num > 0)

    let userHint = hint orelse #stringof(T)
    let ptr      = cast([*] T) alloc(#sizeof(T) * num, allocator, alignment, userHint, loc)
    @init(ptr, num)
    return ptr
}

// Drop and release memory of an array of the given type
#[Swag.Overload]
func(T) delete(ptr: [*] T, num: u64, allocator: IAllocator = null, hint: string = null, loc = #callerlocation)
{
    if !ptr do
        return
    Debug.assert(num > 0)

    let userHint = hint orelse #stringof(T)
    assertIsAllocated(ptr, #sizeof(T) * num, allocator, userHint, loc)
    @drop(ptr, num)
    free(ptr, #sizeof(T) * num, allocator, userHint, loc)
}
