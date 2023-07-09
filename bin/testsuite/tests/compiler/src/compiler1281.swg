#[Swag.Mixin]
func toto(x: code)
{
    var AAA = #mixin x
    @assert(AAA)
}

//#[Swag.PrintBc]
#test
{
    var a = 1
    var b = 2
    toto(#code a < b)
}