#module module840

//#[swag.printbc]
#test
{
	i := 0
	while(i < 10)
	{
		defer @assert(i != 0)
		i += 1
		break
	}
}