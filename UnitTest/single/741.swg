#module module741

//#[swag.printbc]

func(T) toto(x: [..] *T)
{
	@assert(x[0,0] == 10)
}
	
#test
{
	var cpt: [1] s32 = [10]
	var ptr: [1] *s32 = [&cpt]
	toto(ptr)
}
