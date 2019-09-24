#module module948
using swag

//#[swag.printbc]
#test
{
	var t: ()->s32
	ptr := cast(const *typeinfo_func) @typeof(t)
	@assert(@countof(ptr.parameters) == 0)
	//@assert(ptr.return_type == @typeof(s32))
}
