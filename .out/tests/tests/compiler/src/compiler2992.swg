using Swag

#test
{
    @getcontext().tempAllocator.setCapacity(10)
    itf := cast(IAllocator) @getcontext().tempAllocator

    var req: AllocatorRequest
    req.mode = .Alloc
    req.size = 100
    itf.alloc(&req)
    ptr1 := req.address
    @assert(@getcontext().tempAllocator.maxLeak == 100)
    itf.alloc(&req)
    ptr2 := req.address
    @assert(@getcontext().tempAllocator.maxLeak == 200)

    req.mode = .Free
    req.address = ptr1
    req.size = 100
    itf.alloc(&req)
    @assert(@getcontext().tempAllocator.totalLeak == 100)
    req.address = ptr2
    itf.alloc(&req)
    @assert(@getcontext().tempAllocator.totalLeak == 0)
}