using Swag

#[AttrUsage(AttributeUsage.FunctionParameter)]
attr Toto()

func toto(#[Toto] x, y: s32) {}

#[Swag.Optimize(.All, false)]
#test
{
    let t = #typeof(toto)
    @assert(@countof(t.parameters[0].attributes) == 1)
    @assert(@countof(t.parameters[1].attributes) == 1)
}
