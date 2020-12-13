
using swag

func toto(x: s32, y: bool)->s32 => x
func toto(x: s32, y: s32)->s32 => x

#unittest error
//#[swag.printbc]
#test
{
	ptr := cast(const *TypeInfoFunc) @typeof(toto)
}
