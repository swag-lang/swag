#[Swag.Mixin]
func toto(x: code)
{
    for 10:
        #mixin x
}

//#[Swag.PrintBc]
#test
{
    var b = 0
    toto(#code{b += 1;})
    @assert(b == 10)
}