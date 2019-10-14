

//#[swag.printbc]
#test
{
	j := 0
	if(j == 0)
	{
		i := 0
		defer @assert(i == 1)
		i = 1
	}
}