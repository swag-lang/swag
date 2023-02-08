//#global skip
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
	r := ref &ptr
	toto(&r)
	
	//@print(dref ptr)
	@assert(dref ptr == 5)
	@assert(dref r == 5)
}
