

//#[Swag.printbc]
#test
{
	a := 1
	defer a = 0
	defer @assert(a == 1)
}