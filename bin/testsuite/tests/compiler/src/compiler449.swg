

//#[Swag.PrintBc]
#global testerror "San0006"
#test
{
	let a: [..] s32 = null;
    #[Swag.Safety("", true)]
	if a[0] == 0
	{
	}
}