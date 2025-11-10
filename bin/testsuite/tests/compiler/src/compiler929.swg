//#[Swag.PrintBc]
func average(x: any)
{
    let tt = @kindof(x)
    @assert(tt.fullname == "s32")

    using Swag.TypeInfoKind
    @assert(tt.kind == Native)
    @assert(tt.sizeof == 4)
}

//#[Swag.PrintBc]
#test
{
    average(666)
}
