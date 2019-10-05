#module module948
using swag

//#[swag.printbc]
#test
{
	var t: ()->s32
	ptr := cast(const *TypeinfoFunc) @typeof(t)
	@assert(@countof(ptr.parameters) == 0)
	@assert(ptr.returnType == @typeof(s32))
}
