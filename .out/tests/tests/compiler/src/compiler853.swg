

var cpt: s32 = 0

//#[Swag.printbc]
func toto()
{
	defer cpt -= 1
	{
		defer cpt -= 1
		{
			defer cpt = 3
			return;
		}
	}
}

#test
{
	cpt = 0
	toto()
    //@print(cast(s64) cpt)
	@assert(cpt == 1)
}