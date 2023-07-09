#[Swag.Mixin]
func toto()->s32
{
    var x = 666
    return 555
}

#test
{
    var v = toto()
    @assert(v == x - 111)
}