#test
{
    var x: func(x: s32, y: s32)

    var t = @typeof(x)
    @assert(t.parameters[0].name == "x")
    @assert(t.parameters[1].name == "y")
}
