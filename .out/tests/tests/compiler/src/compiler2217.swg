#[Swag.mixin]
func toto()->s32
{
    x := 666
    return 555
}

#test
{
    v := toto()
    @assert(v == x - 111)
}