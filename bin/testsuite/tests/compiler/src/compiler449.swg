

//#[Swag.PrintBc]
#global testerror
#test
{
	let a: [..] s32 = null;
    #[Swag.Safety("", true)]
	if a[0] == 0
	{
	}
}