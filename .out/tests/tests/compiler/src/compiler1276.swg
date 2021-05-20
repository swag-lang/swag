#test
{
    b := 666
    toto()
    @assert(b == 676)
}

#[Swag.mixin]
func toto()
{
    loop 10
        b += 1
}
