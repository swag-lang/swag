#global testerror "Err0309"
#test
{
	var cpt = 0
	switch(5)
	{
	case 5:
		cpt = cast(s32) #index;
	}
}