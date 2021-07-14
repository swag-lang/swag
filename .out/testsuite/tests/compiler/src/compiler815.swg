

func toto(x: const [4] s32)
{
}

//#[Swag.PrintBc]
#test
{
	const a = @[0,1,2,3]
	toto(a)
}
