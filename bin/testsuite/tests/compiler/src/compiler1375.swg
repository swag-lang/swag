#global testerror "Err0682"
#global testerror "Err0434"
#global testerror "Err0699"

using Swag

var GG = false

struct MyAlloc
{
}

impl IAllocator for MyAlloc
{
    func req(self, req: *AllocatorRequest)
    {
        GG = true
        @assert(req.size == 10)
    }
}

func toto()
{
    let cxt = @getcontext()
    var req: AllocatorRequest
    req.size = 10
    cxt.allocator.req(&req)
}

#[Swag.Macro]
func changeContext(user: #code void)
{
    var savedCxt = @getcontext()
    var cxt: Context = dref savedCxt
    @setcontext(cxt)

    #macro
    {
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