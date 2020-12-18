//#[swag.printbc]
#test
{
    var tt: [2] *void
    ptr := &tt[1]
    ptr[-1] = ptr
    @assert(tt[0] == ptr)
}