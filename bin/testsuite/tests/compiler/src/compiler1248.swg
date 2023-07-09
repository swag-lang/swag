using Swag

var GG = false

struct MyAlloc
{
}

impl IAllocator for MyAlloc
{
    func impl req(self, req: *AllocatorRequest)
    {
        GG = true
        @assert(req.size == 10)
    }
}

func toto()
{
    var cxt = @getcontext()

    var req: AllocatorRequest
    req.size = 10
    cxt.allocator.req(&req)
}

#test
{
    GG = false

    var savedCxt = @getcontext()
    var cxt: Context = dref savedCxt
    var myAlloc: MyAlloc
    cxt.allocator = myAlloc
    defer @setcontext(savedCxt)
    @setcontext(cxt)

    toto()
    @assert(GG == true)
}