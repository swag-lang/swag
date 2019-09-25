#module module950
using swag

func toto(x: s32, y: bool)->s32 => x

//#[swag.printbc]
#test
{
	var t: (s32, bool)->s32 = &toto	
	ptr := cast(const *typeinfo_func) @typeof(t)
	@assert(@countof(ptr.parameters) == 2)
	@assert(ptr.return_type == @typeof(s32))
}
