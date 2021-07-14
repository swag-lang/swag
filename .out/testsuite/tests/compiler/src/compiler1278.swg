#[Swag.Mixin]
func toto(x: code)
{
    loop 10
    {
        #mixin x
    }
}

//#[Swag.PrintBc]
#test
{
    b := 0
    toto(#code{b += 1;})
    @assert(b == 10)
}