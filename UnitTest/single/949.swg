#module module949
using swag

func toto(x: s32, y: bool)->s32 => x

//#[swag.printbc]
#test
{
	var t: (s32, bool)->s32 = &toto
	t1 := t
	@assert(t1(10, true) == 10)
	
	//ptr := @typeof(t)
	//ptr := cast(const *TypeinfoFunc) @typeof(t)
	//@assert(@countof(ptr.parameters) == 2)
	//@assert(ptr.return_type == @typeof(f32))
}
