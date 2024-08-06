#global testerror "Err0028"

using Swag

#[Swag.Overload]
func toto(x: s32, y: bool)->s32 => x
#[Swag.Overload]
func toto(x: s32, y: s32)->s32 => x

//#[Swag.PrintBc]
#test
{
	var ptr = cast(const *TypeInfoFunc) @typeof(toto)
}
