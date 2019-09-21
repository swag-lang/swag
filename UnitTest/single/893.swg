#module module893

func toto(x: [..] f32)
{
	@assert(x[0] == 1)
}

//#[swag.printbc]
#test
{
	toto([1])
}