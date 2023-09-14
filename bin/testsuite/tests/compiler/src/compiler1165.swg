#global testerror "Err0228"
#global testerror "Err0133"

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
