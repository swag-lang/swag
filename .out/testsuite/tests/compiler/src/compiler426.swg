


//#[Swag.PrintBc]
#test
{
	res := 0;
	switch(5)
	{
	case 4: @assert(false);
	case 5: res = 5;
	}

	@assert(res == 5);
}