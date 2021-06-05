

//#[Swag.printbc]
func toto(x: const [..] s32)
{
	@assert(@countof(x) == 2)
}

//#[Swag.printbc]
#test
{
	array := @[1 , 2]
	toto(array)
	toto(@[4, 5])
}
