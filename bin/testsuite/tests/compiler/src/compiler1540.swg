#[Swag.ExportType(.Methods)]
struct X
{
    x:   s32
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

//#[Swag.PrintBc]
#test
{
    using Swag
    let ptr = cast(const *TypeInfoStruct) #typeof(X)
    @assert(@countof(ptr.methods) == 2)

    foreach i in ptr.methods
    {
        @assert(i.name == "toto" or i.name == "titi")
        let ptrFunc = cast(const *TypeInfoFunc) i.pointedType
        @assert(ptrFunc.returnType != null)
    }
}
