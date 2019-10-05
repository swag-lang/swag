#module module953
using swag

func toto(x: s32, y: bool)->s32 => x

#unittest error
//#[swag.printbc]
#test
{
	ptr := cast(const *TypeinfoFunc) @typeof(toto + 1)
}
