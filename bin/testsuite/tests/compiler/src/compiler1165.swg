#global testerror "Err0720"
#global testerror "Err0738"

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

#if TOTO
{
	func toto()
	{
	}
}

#test
{
	toto()
}
