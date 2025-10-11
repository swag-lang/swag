using Swag

#test
{
    var alloc: Swag.ScratchAllocator
    alloc.setCapacity(10, @sysalloc())
    let itf = cast(IAllocator) alloc

    var req: AllocatorRequest
    req.mode = .Alloc
    req.size = 100
    itf.req(&req)
    let ptr1 = req.address
    @assert(alloc.maxLeak == 100)
    itf.req(&req)
    let ptr2 = req.address
    @assert(alloc.maxLeak == 200)

    req.mode    = .Free
    req.address = ptr1
    req.size    = 100
    itf.req(&req)
    @assert(alloc.totalLeak == 100)
    req.address = ptr2
    itf.req(&req)
    @assert(alloc.totalLeak == 0)
}
