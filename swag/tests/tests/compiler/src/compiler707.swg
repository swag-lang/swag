

//#[swag.printbc]
#test
{
	cpt := 0
	loop i: 2
	{
		cpt += i
		loop j: 4
		{
			cpt += j
		}
	}

	@assert(cpt == 13)
}