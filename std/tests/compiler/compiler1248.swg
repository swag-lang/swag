using swag

var GG = false

func alloc(req: *AllocatorRequest)
{
    GG = true
    @assert(req.size == 10)
}

func toto()
{
    cxt := @getcontext()
    
    var req: AllocatorRequest
    req.size = 10
    cxt.allocator(&req)
}

#test
{   
    GG = false
    
    savedCxt := @getcontext()
    var cxt: Context = deref savedCxt
    cxt.allocator = &alloc
    defer @setcontext(savedCxt)
    @setcontext(cxt)
    
    toto()
    @assert(GG == true)
}