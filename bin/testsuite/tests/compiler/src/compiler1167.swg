const TOTO = false
const TITI = true

#if TOTO
{
	func toto()
	{
		@assert(false)
	}
}
#elif TOTO
{
	func toto()
	{
		@assert(false)
	}
}
#elif TITI and !TOTO
{
	func toto()
	{
	}
}

#test
{
	toto()
}
