

var cpt: s32 = 0

#test
{
	cpt = 0
	for(10)
	{
		defer cpt -= 1
		{
			defer cpt += 2
			continue
		}
	}

	@assert(cpt == 10)
}