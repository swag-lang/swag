struct S {}

var G = 0

impl S
{
    #[Swag.Macro, Swag.CalleeReturn]
    func(ptr: bool, back: bool) opVisit(me, stmt: #code void)
    {
        var #alias0 = 0
        #if #up ptr do
            G += 1
        #else do
            G += 2
    }
}

//#[Swag.PrintBc]
#test
{
    G = 0

    var x: S
    foreach x
    {
    }
    @assert(G == 2)
    foreach x
    {
    }
    @assert(G == 4)
    foreach &xxx in x
    {
    }
    @assert(G == 5)
}
