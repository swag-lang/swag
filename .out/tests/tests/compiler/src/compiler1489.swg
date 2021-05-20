struct S
{

}

var G = 0

impl S
{
    #[Swag.macro, Swag.noreturn]
    func(ptr: bool) opVisit(using self, stmt: code)
    {
        var @alias0 = 0
        #if `ptr
            G += 1
        #else
            G += 2
    }
}

//#[Swag.printbc]
#test
{
    G = 0

    var x: S
    visit x {}
    @assert(G == 2)
    visit x {}
    @assert(G == 4)
    visit *xxx: x {}
    @assert(G == 5)
}