using Swag

#[AttrUsage(AttributeUsage.EnumValue)]
attr NC1(x: string)

enum A
{
    #[NC1("TOTO")]
    VAL
}

#test
{
    let ptr  = cast(const *TypeInfoEnum) #typeof(A.VAL)
    let ptr1 = cast(const *TypeValue) ptr.values[0]

    //@print(ptr1.attributes[0].item0)
    @assert(ptr1.attributes[0].type == #typeof(NC1))
    @assert(ptr1.attributes[0].params[0].name == "x")

    let val = ptr1.attributes[0].params[0].value
    @assert(@kindof(val) == string)
    @assert(cast(string) val == "TOTO")
}
