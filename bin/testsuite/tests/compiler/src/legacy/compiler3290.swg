#global skip
struct X
{
    x: s32 = 1
    y: s32 = 10
    MK_GET(|x|)
    MK_GET(|y|)
}

#[Swag.Mixin]
func MK_GET()
{
    #run
    {
        let itf = @compiler
        itf.compileString("impl X {\n mtd get_" ++ #nameof(#alias0) ++ "() => " ++ #nameof(#alias0) ++ ";\n}")
    }
}

#test
{
    var v: X
    @assert(v.get_x() == 1)
    @assert(v.get_y() == 10)
}