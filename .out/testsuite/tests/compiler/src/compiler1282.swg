#[Swag.mixin]
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
    a := 1
    b := 2
    toto(#code a < b)
}