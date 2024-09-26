func toto(x: const nn [..] f32)
{
	@assert(x[0] == 1)
}

//#[Swag.PrintBc]
#test
{
	toto([1])
}