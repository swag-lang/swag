

//#[Swag.printbc]
#test
{
	a := 0
	loop(1)
	{
		defer a = 1
		break
	}
	@assert(a == 1)
}