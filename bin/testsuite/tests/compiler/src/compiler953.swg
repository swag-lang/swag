#global testerror

using Swag

func toto(x: s32, y: bool)->s32 => x

//#[Swag.PrintBc]
#test
{
	ptr := cast(const *TypeInfoFunc) @typeof(toto + 1)
}
