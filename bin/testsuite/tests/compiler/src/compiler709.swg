#global testerror "Err0028"
//#[Swag.PrintBc]
#test
{
	loop i: 2
	{
		loop i: 2
		{
            @print(i)
		}
	}
}