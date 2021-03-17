const TOTO = false
const TITI = true

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
#else #if TITI && !TOTO
{
	func toto()
	{
	}
}

#test
{
	toto()
}
