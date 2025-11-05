using Swag

#[AttrUsage(AttributeUsage.Enum)]
attr NC1(x: string)

#[NC1("toto")]
enum A
{
    B
}

#test
{
    let ptr = cast(const *TypeInfoEnum) #typeof(A)
    @assert(@countof(ptr.attributes) == 1)
    @assert(ptr.attributes[0].type == #typeof(NC1))
    @assert(ptr.attributes[0].params[0].name == "x")

    let val = ptr.attributes[0].params[0].value
    @assert(@kindof(val) == #typeof(string))
    let ptrVal = cast(const *string) @dataof(val)
    @assert(dref ptrVal == "toto")
}
