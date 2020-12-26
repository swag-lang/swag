

func toto(x: const [4] s32)
{
}

//#[swag.printbc]
#test
{
	const a = @[0,1,2,3]
	toto(a)
}
