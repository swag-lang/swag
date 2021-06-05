//#[Swag.printbc]
#test
{
    var tt: [2] *void
    ptr := &tt[1]
    ptr[bitcast(uint) -1'int] = ptr
    @assert(tt[0] == ptr)
}