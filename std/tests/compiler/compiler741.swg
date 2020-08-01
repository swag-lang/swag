func(T) toto(x: [..] T)
{
	@assert(@countof(x) == 1)
	@assert(x[0] == 10)
}

//#[swag.printbc]
#test
{
	var cpt: [1] s32 = @[10]
	toto(cast([..] s32) cpt)
}
