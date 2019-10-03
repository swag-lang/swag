#module module1014
using swag

func toto(x: s32, y: bool)->s32 => x

//#[swag.printbc]
#test
{
	ptr := cast(const *typeinfo_func) @typeof(toto)
	//@assert(ptr.return_type == @typeof(s32))
	//@assert(ptr.return_type is const *typeinfo)
	@assert(@typeof(ptr.return_type) == @typeof(const *typeinfo))
}
