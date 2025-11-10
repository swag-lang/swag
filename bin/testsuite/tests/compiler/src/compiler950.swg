using Swag

func toto(x: s32, y: bool)->s32 => x

//#[Swag.PrintBc]
#test
{
    let t:  func(s32, bool)->s32 = &toto
    let ptr = cast(const *TypeInfoFunc) #typeof(t)
    @assert(@countof(ptr.parameters) == 2)
    @assert(ptr.returnType == #typeof(s32))
}
