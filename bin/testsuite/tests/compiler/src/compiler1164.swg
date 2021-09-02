const TOTO = false
#if TOTO
{
	#if TOTO
	{
		func toto()
		{
			@assert(false)
		}
	}
}
#else
{
	func toto()
	{
	}
}

#test
{
	toto()
}
