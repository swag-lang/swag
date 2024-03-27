
using Core, Swag

struct My
{

}

var G = 0

impl IAllocator for My
{
    func impl req(self, req: *AllocatorRequest)
    {
        var sys: SystemAllocator
        let itf = cast(IAllocator) sys
        G = 1
        itf.req(req)
    }
}

#test
{
    var v: My
    Memory.pushAllocator(v)
    {
        const value = "this is a string this is a string this is a string"
        var s: String = value
        @assert(s == value)
    }

    @assert(G == 1)
}