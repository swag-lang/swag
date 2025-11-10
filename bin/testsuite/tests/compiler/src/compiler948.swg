using Swag

//#[Swag.PrintBc]
#test
{
    var t:  func()->s32
    let ptr = cast(const *TypeInfoFunc) #typeof(t)
    @assert(@countof(ptr.parameters) == 0)
    @assert(ptr.returnType == #typeof(s32))
}
