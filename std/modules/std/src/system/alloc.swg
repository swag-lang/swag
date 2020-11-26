public namespace std.memory;
using swag

// Free the specified memory block
func free(buffer: *void, allocator: IAllocator = null, loc = #callerlocation)
{
    var req: AllocatorRequest
    req.callerLoc   = loc
    req.mode        = AllocatorMode.Free
    req.address     = buffer

    talloc := allocator ?? @getcontext().allocator
    talloc.alloc(&req)
}

// Free all allocated memory if possible
func freeAll(allocator: IAllocator, loc = #callerlocation)
{
    var req: AllocatorRequest
    req.callerLoc   = loc
    req.mode        = AllocatorMode.FreeAll

    talloc := allocator ?? @getcontext().allocator
    talloc.alloc(&req)
}

// Reallocate/allocate a given amount of bytes
func realloc(buffer: *void, sizeInBytes: u32, allocator: IAllocator = null, alignement: u16 = 0, loc = #callerlocation)->*void
{
    var req: AllocatorRequest
    req.callerLoc   = loc
    req.mode        = AllocatorMode.Realloc
    req.address     = buffer
    req.size        = sizeInBytes
    req.alignement  = alignement

    talloc := allocator ?? @getcontext().allocator
    talloc.alloc(&req)

    return req.address
}

// Allocate a given amount of bytes
func alloc(sizeInBytes: u32, allocator: IAllocator = null, alignement: u16 = 0, loc = #callerlocation)->*void
{
    var req: AllocatorRequest
    req.callerLoc   = loc
    req.mode        = AllocatorMode.Alloc
    req.size        = sizeInBytes
    req.alignement  = alignement

    talloc := allocator ?? @getcontext().allocator
    talloc.alloc(&req)

    return req.address
}

func(T) new(allocator: IAllocator = null, alignement: u16 = 0, loc = #callerlocation)->*T
{
    ptr := cast(*T) alloc(@sizeof(T), allocator, alignement, loc)
    @init(ptr)
    return ptr
}

func(T) delete(ptr: *T, allocator: IAllocator = null, loc = #callerlocation)
{
    @drop(ptr)
    free(ptr, allocator, loc)
}