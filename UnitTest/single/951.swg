#module module951
using swag

func toto(x: s32, y: bool)->s32 => x
//func toto(x: s32, y: s32)->s32 => x

//#[swag.printbc]
#test
{
	ptr := cast(const *typeinfo_func) @typeof(toto)
	@assert(@countof(ptr.parameters) == 2)
	@assert(ptr.return_type == @typeof(s32))
}
