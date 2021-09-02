//#[Swag.PrintBc]
#test
{
    b := true
    @assert(b == true)
    b &= false
    @assert(b == false)
    b |= true
    @assert(b == true)
}