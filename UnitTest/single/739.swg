#module module739

//#[swag.printbc]

func(T) toto(x: [] *T)
{
/*	@assert(x[0] == 1)
	@assert(x[1] == 2)
	@assert(x[2] == 3)
	@assert(x[3] == 4)*/
}
	
#test
{
	var cpt: [] s32 = [1]
	var ptr: [] *s32 = [&cpt]
	toto(ptr)
}
