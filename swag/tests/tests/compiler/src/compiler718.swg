//

//#[swag.printbc]
//#unittest error
#test
{
	var ptr1: *void = null
	ptr1 += 1
    @assert(cast(uint) ptr1 == 1)
}