#global skip
using Swag

#test
{
    var dbg: DebugAllocator
    dbg.setup(systemAllocator())

    var req: AllocatorRequest
    req.size = 10
    req.callerLoc = #location
    dbg.IAllocator.req(&req)

    req1 := req
    req1.mode = .Free
    dbg.IAllocator.req(&req1)

    req2 := req
    req2.mode = .Free
    dbg.IAllocator.req(&req2)
}