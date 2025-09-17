func(T, N: u32) new()->[*] T
{
    var req: Swag.AllocatorRequest
    req.size = #sizeof(T) * N
    @getcontext().allocator.req(&req)

    @init(cast([*] T) req.address, N)

    return cast() req.address
}

func(T, N: u32) delete(addr: [*] T)
{
    @drop(addr, N)

    var req: Swag.AllocatorRequest
    req.size    = #sizeof(T) * N
    req.address = addr
    req.mode    = Swag.AllocatorMode.Free
    @getcontext().allocator.req(&req)
}

struct Point { x, y: string = "coucou" }

var G: s32 = 0

impl Point
{
    func opDrop(me)
    {
        G += 1
    }
}

//#[Swag.PrintBc]
#test
{
    G = 0
    let ptr = new'(Point, 4)()
    delete'(Point, 4)(ptr)
    @assert(G == 4)
}
