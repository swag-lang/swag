using Swag

/*func toto()
{
    let cxt = @getcontext()

    var req: AllocatorRequest
    req.size = 10
    cxt.allocator(&req)
}*/

#test
{
    //toto()

    var cxt: Context
    cxt = dref @getcontext()

    var req: AllocatorRequest
    req.size = 10
    @assert(req.address == null)
    cxt.allocator.req(&req)
    @assert(req.address != null)

    req.mode = AllocatorMode.Free
    cxt.allocator.req(&req)
}
