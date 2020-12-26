
using swag

//#[swag.printbc]
#test
{
	var t: func()->s32
	ptr := cast(const *TypeInfoFunc) @typeof(t)
	@assert(@countof(ptr.parameters) == 0)
	@assert(ptr.returnType == @typeof(s32))
}
