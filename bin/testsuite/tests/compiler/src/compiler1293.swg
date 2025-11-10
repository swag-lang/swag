#[Swag.Mixin]
func toto(y: s32)
{
    x = y
}

#test
{
    var x = 0
    toto(6)
    @assert(x == 6)
}
