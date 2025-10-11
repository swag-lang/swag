/*#[Swag.Mixin]
func MK_IMPL()
{
    #ast
    {
        return "x: s32 = 666"
    }
}

struct X
{
    MK_IMPL()
}

#test
{
    var v: X
    @assert(v.x == 666)
}*/
