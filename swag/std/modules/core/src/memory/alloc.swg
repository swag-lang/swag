#global public
#global namespace memory
using swag

// Free the specified memory block
func free(buffer: *void, size: uint, allocator: IAllocator = null, hint: string = null, loc = #callerlocation)
{
    if !buffer return

    var req: AllocatorRequest
    req.callerLoc   = loc
    req.mode        = AllocatorMode.Free
    req.size        = size
    req.address     = buffer
    req.hint        = hint

    talloc := allocator ?? @getcontext().allocator
    talloc.alloc(&req)
}

// Free all allocated memory of the given allocator if possible
func freeAll(allocator: IAllocator, hint: string = null, loc = #callerlocation)
{
    var req: AllocatorRequest
    req.callerLoc   = loc
    req.mode        = AllocatorMode.FreeAll
    req.hint        = hint

    talloc := allocator ?? @getcontext().allocator
    talloc.alloc(&req)
}

// Reallocate/allocate a given amount of bytes
func realloc(buffer: *void, newSize: uint, oldSize: uint, allocator: IAllocator = null, alignement: u16 = 0, hint: string = null, loc = #callerlocation)->*void
{
    var req: AllocatorRequest
    req.callerLoc   = loc
    req.hint        = hint
    req.mode        = AllocatorMode.Realloc
    req.address     = buffer
    req.size        = newSize
    req.oldSize     = oldSize
    req.alignement  = alignement

    talloc := allocator ?? @getcontext().allocator
    talloc.alloc(&req)

    return req.address
}

// Allocate a given amount of bytes
func alloc(size: uint, allocator: IAllocator = null, alignement: u32 = 0, hint: string = null, loc = #callerlocation)->*void
{
    var req: AllocatorRequest
    req.callerLoc   = loc
    req.hint        = hint
    req.mode        = AllocatorMode.Alloc
    req.size        = size
    req.alignement  = alignement

    talloc := allocator ?? @getcontext().allocator
    talloc.alloc(&req)

    return req.address
}

// Allocate and initialize the given type
func(T) new(allocator: IAllocator = null, alignement: u32 = 0, hint: string = null, loc = #callerlocation)->*T
{
    userHint := hint ?? @typeof(T).name
    ptr := cast(*T) alloc(@sizeof(T), allocator, alignement, userHint, loc)
    @init(ptr)
    return ptr
}

// Drop and release memory of the given type
func(T) delete(ptr: *T, allocator: IAllocator = null, hint: string = null, loc = #callerlocation)
{
    if !ptr
        return
    userHint := hint ?? @typeof(T).name
    @drop(ptr)
    free(ptr, @sizeof(T), allocator, userHint, loc)
}

// Allocate and initialize an array of the given type
func(T) new(num: uint, allocator: IAllocator = null, alignement: u32 = 0, hint: string = null, loc = #callerlocation)->*T
{
    debug.assert(num > 0)

    userHint := hint ?? @typeof(T).name
    ptr := cast(*T) alloc(@sizeof(T) * num, allocator, alignement, userHint, loc)
    @init(ptr, num)
    return ptr
}

// Drop and release memory of an array of the given type
func(T) delete(ptr: *T, num: uint, allocator: IAllocator = null, hint: string = null, loc = #callerlocation)
{
    if !ptr
        return
    debug.assert(num > 0)

    userHint := hint ?? @typeof(T).name
    @drop(ptr, num)
    free(ptr, @sizeof(T) * num, allocator, userHint, loc)
}