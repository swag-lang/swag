#test
{
    b := 666
    toto()
    @assert(b == 676)
}

#[Swag.Mixin]
func toto()
{
    loop 10
        b += 1
}
