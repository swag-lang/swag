#module "compiler440"

//#[swag.printbc]
#unittest error
#test
{
	cpt := 0
	switch(5)
	{
	case 5: 
		cpt = cast(s32) @index;
	}
}