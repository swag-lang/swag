using Swag

#[attributeUsage(AttributeUsage.Function)]
attr Command();

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
    ptr := cast(const* TypeInfoStruct) @typeof(A)
    @assert(@countof(ptr.methods) == 1)

    fct := ptr.methods[0]
    @assert(fct.name == "toto")
    @assert(@countof(fct.attributes) == 1)
}