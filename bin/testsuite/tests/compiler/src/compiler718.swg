//

//#[Swag.PrintBc]
//#global testerror
#test
{
	var ptr1: ^u8 = null
	ptr1 += 1
    @assert(cast(uint) ptr1 == 1)
}