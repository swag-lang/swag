#global skip
#global testerror;

//#[Swag.PrintBc]
#test
{
	var a: string = null;
    #[Swag.Safety(.All, .All, true)]
	if(a[0] == 1)
	{
	}
}