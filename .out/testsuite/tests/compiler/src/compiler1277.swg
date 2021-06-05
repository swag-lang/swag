#[Swag.mixin]
func toto(x: code)
{
    #mixin x
    #mixin x
    #mixin x
}

//#[Swag.printbc]
#test
{
    b := 0
    toto(#code{b += 1;})
    @assert(b == 3)
}