struct X
{
    x: s32
}

impl X
{
    func toto()->X
    {
        return @{x: 666}
    }

    func titi()->X
    {
        return @{x: 666}
    }
}

//#[swag.printbc]
#test
{
    using swag
    ptr := cast(const *TypeInfoStruct) @typeof(X)
    @assert(@countof(ptr.methods) == 2)

    visit i: ptr.methods
    {
        @assert(i.name == "toto" || i.name == "titi")
        ptrFunc := cast(const *TypeInfoFunc) i.pointedType
        @assert(ptrFunc.returnType != null)
    }
}