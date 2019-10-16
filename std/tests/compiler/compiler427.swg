


//#[swag.printbc]
#test
{
	res := 0;
	switch(50)
	{
	case 4: 	@assert(false);
	case 5: 	res = 5;
	default: 	res += 1;
	}
	
	@assert(res == 1);
}