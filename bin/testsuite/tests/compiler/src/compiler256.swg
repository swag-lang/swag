using Swag;

#test
{
	var i = 5;
	while i > 0
	{
		i -= 1;
	}

	@assert(i == 0);
}
