#module module853

var cpt: s32 = 0

//#[swag.printbc]
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
	@assert(cpt == 1)	
}