//#[Swag.PrintBc]
#test
{
    var x   = [1, 2, 3]
    let ptr = &x
    @init(ptr, 2)
    @assert(x[0] == 0)
    @assert(x[1] == 0)
    @assert(x[2] == 3)
}
