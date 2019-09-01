#module module741

func toto(x: [..] s32)
{
	@assert(@prop(x, count) == 1)
	@assert(x[0] == 10)
}
	
//#[swag.printbc]
#test
{
	var cpt: [1] s32 = [10]
	toto(cast([..] s32) cpt)
}
