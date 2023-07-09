#[Swag.Mixin]
func toto(x: code)
{
    if #mixin x
        @assert(true)
    else
        @assert(false)
}

//#[Swag.PrintBc]
#test
{
    var a = 1
    var b = 2
    toto(#code a < b)
}