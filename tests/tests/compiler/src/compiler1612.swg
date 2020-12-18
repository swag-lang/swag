//#[swag.printbc]
#test
{
    var ptr:  *u8 = null
    var ptr1: const *u8 = null
    @assert(ptr == ptr1)
}