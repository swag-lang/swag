#global testerror "Err0708"
#global testerror "Err0723"

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
