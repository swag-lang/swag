#global skip
var G: u64 = 666
func toto()->const &u64
{
    return G
}

//#[Swag.PrintBc]
#test
{
    var x = toto()
    @assert(#typeof(x) == u64)
    @assert(x == 666)
}