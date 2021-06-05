#[Swag.mixin]
func toto(y: s32)
{
    x = y
}

#test
{
    x := 0
    toto(6)
    @assert(x == 6)
}