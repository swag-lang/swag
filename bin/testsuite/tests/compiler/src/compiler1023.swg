//#[Swag.PrintBc]
#test
{
    var x   = 1
    let ptr = &x
    @init(ptr, 1)
    @assert(x == 0)
}
