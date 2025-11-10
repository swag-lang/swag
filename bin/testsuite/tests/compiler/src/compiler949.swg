using Swag

func toto(x: s32, y: bool)->s32 => x

//#[Swag.PrintBc]
#test
{
    let t: func(s32, bool)->s32 = &toto
    let t1 = t
    @assert(t1(10, true) == 10)

    //var ptr = #typeof(t)
    //var ptr = cast(const *TypeInfoFunc) #typeof(t)
    //@assert(@countof(ptr.parameters) == 2)
    //@assert(ptr.returnType == #typeof(f32))
}
