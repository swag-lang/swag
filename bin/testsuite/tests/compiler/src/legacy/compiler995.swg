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
    let ptr = cast(const *TypeInfoEnum) #typeof(A.VAL)
    @assert(@countof(ptr.values) == 1)
    @assert(@countof(ptr.attributes) == 0)

    let ptr1 = cast(const *TypeValue) ptr.values[0]
    @assert(@countof(ptr1.attributes) == 1)
    @assert(ptr1.attributes[0].type == #typeof(NC1))
    @assert(ptr1.attributes[0].params[0].name == "x")

    let val = ptr1.attributes[0].params[0].value
    @assert(@kindof(val) == #typeof(string))
    let ptrVal = cast(*string) @dataof(val)
    @assert(dref ptrVal == "TOTO")
}
