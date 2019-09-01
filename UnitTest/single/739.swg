#module module739

//#[swag.printbc]

func(T) toto(x: [1] *T)
{
	@assert(x[0][0] == 1)
}
	
#test
{
	var cpt: [] s32 = [1]
	var ptr: [] *s32 = [&cpt]
	toto(ptr)
}
