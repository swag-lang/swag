


//#[Swag.PrintBc]
#test
{
	const RES = 6;
	var res = 0;
	switch(6)
	{
	case 4: 		@assert(false);
	case 5, RES: 	res = 5;
	default: 		res += 1;
	}

	@assert(res == 5);
}