//#[Swag.PrintBc]
func toto(x: [2] s64)
{
    let ptr = &x[0]
    @assert(ptr[0] == 10)
    @assert(ptr[1] == 20)
}

//#[Swag.PrintBc]
#test
{
    var x = [10's64, 20's64]
    toto(x)
}