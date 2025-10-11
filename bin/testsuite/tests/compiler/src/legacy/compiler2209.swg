//#[Swag.PrintBc]
#test
{
    var b = true
    @assert(b == true)
    b &= false
    @assert(b == false)
    b |= true
    @assert(b == true)
}
