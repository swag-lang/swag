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

#[swag.macro]
func changeContext(user: code)
{
    savedCxt := @getcontext()
    var newCxt: Context = :savedCxt
    @setcontext(newCxt)

    #macro
    {
        let cxt = &`newCxt

        #mixin `user
    }

    @setcontext(savedCxt)
}

#test
{
    GG = false

    changeContext()
    {
        var myAlloc: MyAlloc
        cxt.allocator = myAlloc

        toto()
    }

    @assert(GG == true)
}