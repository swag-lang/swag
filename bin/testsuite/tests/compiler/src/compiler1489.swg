struct S
{

}

var G = 0

impl S
{
    #[Swag.Macro, Swag.CalleeReturn]
    func(ptr: bool, back: bool) opVisit(using self, stmt: code)
    {
        var #alias0 = 0
        #if #up ptr #do
            G += 1
        #else #do
            G += 2
    }
}

//#[Swag.PrintBc]
#test
{
    G = 0

    var x: S
    visit x {}
    @assert(G == 2)
    visit x {}
    @assert(G == 4)
    visit &xxx: x {}
    @assert(G == 5)
}