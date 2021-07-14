

//#[Swag.PrintBc]
func(T) toto(x: [..] T)
{
	@assert(@countof(x) == 1)
	@assert(x[0] == 10)
}

//#[Swag.PrintBc]
#test
{
	var cpt: [1] s32 = @[10]
	toto(cast([..] s32) cpt)

	var cpt2: [1] f32 = @[10]
	toto(cast([..] f32) cpt2)
}
