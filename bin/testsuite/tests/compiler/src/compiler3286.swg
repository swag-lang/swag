#global skip
#[Swag.Mixin]
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