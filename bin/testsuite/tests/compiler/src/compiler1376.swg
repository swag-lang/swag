using Swag

var GG = false

struct MyAlloc {}

impl IAllocator for MyAlloc
{
    func impl req(me, req: *AllocatorRequest)
    {
        GG = true
        @assert(req.size == 10)
    }
}

func toto()
{
    let cxt  = @getcontext()
    var req: AllocatorRequest
    req.size = 10
    cxt.allocator.req(&req)
}

#[Swag.Macro]
func changeContext(user: #code void)
{
    let savedCxt = @getcontext()
    var newCxt:  Context = dref savedCxt
    @setcontext(newCxt)

    #macro
    {
        var cxt = &#up newCxt

        #inject(#up user)
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
