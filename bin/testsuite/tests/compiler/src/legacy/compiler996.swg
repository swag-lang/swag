using Swag

#[AttrUsage(AttributeUsage.EnumValue)]
attr NC1(x: string)

enum A: f32
{
    #[NC1("TOTO")]
    VAL = 666
}

#test
{
    let ptr = cast(const *TypeInfoEnum) #typeof(A.VAL)
    @assert(@countof(ptr.values) == 1)
    @assert(@countof(ptr.attributes) == 0)
    let ptr1   = cast(const *TypeValue) ptr.values[0]
    let ptrVal = cast(const *f32) ptr1.value
    @assert(dref ptrVal == 666)
}
