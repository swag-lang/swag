struct X
{
    x: s32
}

#if false
{
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
}

impl X
{
    func toto(using const self)
    {

    }
}

#test
{
    using swag
    ptr := cast(const *TypeInfoStruct) @typeof(X)
    @assert(@countof(ptr.methods) == 1)
}