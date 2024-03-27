#global testerror "memory block double free"
using Swag

#test
{
    var dbg: DebugAllocator
    dbg.setup(@sysalloc())

    var req: AllocatorRequest
    req.size = 10
    req.callerLoc = #location
    dbg.IAllocator.req(&req)

    var req1 = req
    req1.mode = .Free
    req1.callerLoc = #location
    dbg.IAllocator.req(&req1)

    var req2 = req
    req2.mode = .Free
    req2.callerLoc = #location
    dbg.IAllocator.req(&req2)
}