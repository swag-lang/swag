

//#[swag.printbc]
#test
{
	loop(1)
	{
		a := 0
		defer @assert(a == 1)
		a += 1
	}
}