using Swag

#[AttrUsage(AttributeUsage.StructVariable)]
attr NC1(x: s32)
#[AttrUsage(AttributeUsage.StructVariable)]
attr NC2(y: s32)

struct Point
{
    #[NC1(666), NC2(666)]
    x:   s32
}

//#[Swag.PrintBc]
#test
{
    let t = cast(const *TypeInfoStruct) #typeof(Point)
    let p = cast(const *TypeValue) t.fields[0]

    //@print(cast(s64) @countof(p.attributes))

    @assert(@countof(p.attributes) == 2)
    @assert(p.attributes[0].type == #typeof(NC1))
    @assert(p.attributes[1].type == #typeof(NC2))

    @assert(@countof(p.attributes[0].params) == 1)
    @assert(@countof(p.attributes[1].params) == 1)

    let ptr = @kindof(p.attributes[1].params[0].value)
    //@print(cast(s64) ptr.sizeof)
    //@print(ptr.name)
    //@print("\n")
    @assert(ptr.fullname == "s32")
    let ptr1 = cast(*s32) @dataof(p.attributes[1].params[0].value)
    @assert(dref ptr1 == 666)
}
