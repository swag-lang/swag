
using swag

func toto(x: s32, y: bool)->s32 => x

//#[swag.printbc]
#test
{
	var t: func(s32, bool)->s32 = &toto	
	ptr := cast(const *TypeInfoFunc) @typeof(t)
	@assert(@countof(ptr.parameters) == 2)
	@assert(ptr.returnType == @typeof(s32))
}
