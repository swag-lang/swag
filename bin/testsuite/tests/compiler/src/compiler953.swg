#global testerror "Err0439"

using Swag

func toto(x: s32, y: bool)->s32 => x

//#[Swag.PrintBc]
#test
{
	var ptr = cast(const *TypeInfoFunc) #typeof(toto + 1)
}
