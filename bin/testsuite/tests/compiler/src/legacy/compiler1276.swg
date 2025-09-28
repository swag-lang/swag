#test
{
    var b = 666
    toto()
    @assert(b == 676)
}

#[Swag.Mixin]
func toto()
{
    for 10:
        b += 1
}
