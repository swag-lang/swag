using Swag

var G = 5

func toto(x: ref *s32)
{
	x = &G
}

#test
{
	a := 10
	ptr := &a
	toto(&ptr)
	
	//@print(dref ptr)
	@assert(dref ptr == 5)
}
