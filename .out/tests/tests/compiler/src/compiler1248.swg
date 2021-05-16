using swag

var GG = false

struct MyAlloc
{
}

impl IAllocator for MyAlloc
{
    func alloc(self, req: *AllocatorRequest)
    {
        GG = true
        @assert(req.size == 10)
    }
}

func toto()
{
    cxt := @getcontext()

    var req: AllocatorRequest
    req.size = 10
    cxt.allocator.alloc(&req)
}

#test
{
    GG = false

    savedCxt := @getcontext()
    var cxt: Context = dref savedCxt
    var myAlloc: MyAlloc
    cxt.allocator = myAlloc
    defer @setcontext(savedCxt)
    @setcontext(cxt)

    toto()
    @assert(GG == true)
}