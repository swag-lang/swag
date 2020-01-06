struct X
{
    x: s32
}

impl X
{
    func toto()->X
    {
        return {x: 666}
    }

    func titi()->X
    {
        return {x: 666}
    }    
}

#test
{
    using swag
    ptr := cast(const *TypeInfoStruct) @typeof(X)
    @assert(@countof(ptr.methods) == 2)
    //@print(cast(s64) @countof(ptr.methods))

    visit i: ptr.methods
    {
        @assert(i.namedParam == "toto" || i.namedParam == "titi")
        ptrFunc := cast(const *TypeInfoFunc) i.pointedType
        //@assert(ptrFunc.returnType != null)
    }
}