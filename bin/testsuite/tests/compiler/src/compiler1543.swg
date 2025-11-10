#[Swag.ExportType(.Methods)]
struct X
{
    x:   s32
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
    func toto(const me, xx: X) {}

    func toto1(const me, xx: X) {}
}

#test
{
    using Swag
    let ptr = cast(const *TypeInfoStruct) #typeof(X)
    @assert(@countof(ptr.methods) == 2)
}
