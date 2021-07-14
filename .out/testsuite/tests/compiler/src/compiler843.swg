

var G: s32 = 0

//#[Swag.PrintBc]
#test
{
	G = 0

	{
		i := 0
		defer @assert(i == 1)
		defer G = 1
		i = 1
	}

	@assert(G == 1)
}