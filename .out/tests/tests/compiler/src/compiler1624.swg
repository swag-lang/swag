var a: u32 = 700

//#[Swag.printbc]
#test
{
	a = 700
    a &= 4
    a |= 1
    a ^= 5
    @assert(a == 0)
}
