func toto() => 3

//#[Swag.PrintBc]
#test
{
    var x: [4] u32 = [0, 1, 2, 3]
    var v = x[toto()]
    @assert(v == 3)
}