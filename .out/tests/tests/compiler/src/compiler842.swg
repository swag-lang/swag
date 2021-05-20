

var G: s32 = 0

//#[Swag.printbc]
#test
{
	G = 0

	j := 1
	if(j == 0)
	{
	}
	else
	{
		i := 0
		defer @assert(i == 1)
		defer G = 1
		i = 1
	}

	@assert(G == 1)
}