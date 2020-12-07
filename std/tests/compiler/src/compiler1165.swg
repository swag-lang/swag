#module "compiler1165"
#unittest error

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
