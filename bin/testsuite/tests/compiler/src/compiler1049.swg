func(T) new()->*T
{
    var req: Swag.AllocatorRequest
    req.size = #sizeof(T)
    @getcontext().allocator.req(&req)

    @init(cast(*T) req.address, 1)()

    return cast() req.address
}

struct Point { x, y: string = "coucou" }

//#[Swag.PrintBc]
#test
{
    let ptr = new'Point()
    @assert(ptr.x == "coucou")
    @assert(ptr.y == "coucou")

    var req: Swag.AllocatorRequest
    req.address = ptr
    req.mode    = .Free
    req.size    = #sizeof(Point)
    @getcontext().allocator.req(&req)
}
