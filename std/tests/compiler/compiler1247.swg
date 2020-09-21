using swag

/*func toto()
{
    cxt := @getcontext()

    var req: AllocatorRequest
    req.size = 10
    cxt.allocator(&req)
}*/

#test
{
    //toto()

    var cxt: Context
    cxt = :@getcontext()

    var req: AllocatorRequest
    req.size = 10
    @assert(req.address == null)
    cxt.allocator.alloc(&req)
    @assert(req.address != null)

    req.mode = AllocatorMode.Free
    cxt.allocator.alloc(&req)
}