using Swag

#[AttrUsage(AttributeUsage.FunctionParameter)]
attr Toto()

func toto(#[Toto] x: s32)
{
}

#[Swag.Optim("", false)]
#test
{
    var t = @typeof(toto)
    @assert(@countof(t.parameters[0].attributes) == 1)
}