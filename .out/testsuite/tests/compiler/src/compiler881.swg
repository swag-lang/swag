

//#[Swag.printbc]
#test
{
	var a = true ? 1 : 10
	@assert(a == 1)
	a = false ? 1 : 10
	@assert(a == 10)
}
