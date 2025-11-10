using Swag

#[AttrUsage(AttributeUsage.StructVariable)]
attr NC1()

struct Point
{
    #[NC1]
    x:   s32

    #[NC1]
    y:   s32
}

//#[Swag.PrintBc]
#test
{
    let t = cast(const *TypeInfoStruct) #typeof(Point)
    @assert(@countof(t.fields) == 2)

    for t.fields
    {
        let p = cast(const *TypeValue) t.fields[@index]
        @assert(p.name == (@index == 0 ? "x" : "y"))
        @assert(@countof(p.attributes) == 1)
        @assert(p.attributes[0].type == #typeof(NC1))
    }
}
