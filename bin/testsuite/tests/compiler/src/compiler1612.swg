//#[Swag.PrintBc]
#test
{
    let ptr:  *u8 = null
    let ptr1: const *u8 = null
    @assert(ptr == ptr1)
}
