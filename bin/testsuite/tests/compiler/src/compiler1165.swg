#global testerror "Err0668"
#global testerror "Err0695"

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
