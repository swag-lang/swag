
#global testpass syntax

#test
{
	for var i = 0; i < 10; i += 10
	{
	}

	for var i = 0; i < 10; i += 10;
	{
	}

	for var i = 0
		i < 10
		i += 10
	{
	}

	for { var i = 0; var j = 1; }
		i < 10
		i += 10
	{
	}

	for { var i = 0; var j = 1; }
		(i < 10)
		{ i += 10; j += 20; }
	{
	}
}