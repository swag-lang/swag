using Swag

#[AttrUsage(AttributeUsage.Function)]
attr Command()

#[Swag.ExportType("methods")]
struct A
{
}

impl A
{
    #[Command]
    func toto()
    {
    }
}

#test
{
    let ptr = cast(const *TypeInfoStruct) @typeof(A)
    @assert(@countof(ptr.methods) == 1)

    let fct = ptr.methods[0]
    @assert(fct.name == "toto")
    @assert(@countof(fct.attributes) == 1)
}
