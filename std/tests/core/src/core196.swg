#import "core"
using core, swag

struct My
{

}

var G = 0

impl IAllocator for My
{
    func alloc(self, req: *AllocatorRequest)
    {
        var sys: SystemAllocator
        var itf = cast(IAllocator) sys
        G = 1
        itf.alloc(req)
    }
}

#test
{
    var v: My
    system.pushAllocator(v)
    {
        const value = "this is a string this is a string this is a string"
        var s: String = value
        @assert(s == value)
    }

    @assert(G == 1)
}