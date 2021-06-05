func toto(x: const [..] f32)
{
	@assert(x[0] == 1)
}

//#[Swag.printbc]
#test
{
	toto(@[1])
}