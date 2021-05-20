

//#[Swag.printbc]
#test
{
	cpt := 0
	{
		defer cpt -= 1
		{
			defer cpt -= 1
			{
				defer cpt = 3
			}
		}
	}

	@assert(cpt == 1)
}