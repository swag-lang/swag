
//#[Swag.PrintBc]
#test
{
	var cpt = 0
	loop i in 2
	{
		cpt += i
		loop j in 4
		{
			cpt += j
		}
	}

	@assert(cpt == 13)
}