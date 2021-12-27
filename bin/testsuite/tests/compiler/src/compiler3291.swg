struct X
{
    x: s32 = 1
    y: s32 = 10
    MK_GET(#code x)
    MK_GET(#code y)
}

#[Swag.Mixin]
func MK_GET(cd: alias)
{
    #run
    {
        itf := @compiler()
        itf.compileString("impl " ~@nameof(#self)~ " {\n mtd get_" ~ @nameof(cd) ~ "() => " ~ @nameof(cd) ~ ";\n}")
    }
}

#test
{
    var v: X
    @assert(v.get_x() == 1)
    @assert(v.get_y() == 10)
}