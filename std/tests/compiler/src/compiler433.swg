
#unittest pass syntax

#test
{
	for i := 0; i < 10; i += 10
	{
	}

	for i := 0; i < 10; i += 10;
	{
	}

	for i := 0
		i < 10
		i += 10
	{
	}

	for { i := 0; j := 1; }
		i < 10
		i += 10
	{
	}

	for { i := 0; j := 1; }
		(i < 10)
		{ i += 10; j += 20; }
	{
	}
}