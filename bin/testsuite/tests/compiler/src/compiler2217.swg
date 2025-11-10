#[Swag.Mixin]
func toto()->s32
{
    let x = 666
    return 555
}

#test
{
    let v = toto()
    @assert(v == x - 111)
}
