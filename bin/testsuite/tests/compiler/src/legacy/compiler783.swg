//#[Swag.PrintBc]
#test
{
    using Swag

    let t = #typeof(u32)
    @assert(t.fullname == "u32")
    @assert(@countof(t.fullname) == 3)
    @assert(t.kind == TypeInfoKind.Native)

    let t1 = #typeof(**bool)
    @assert(t1.fullname == "**bool")
}
