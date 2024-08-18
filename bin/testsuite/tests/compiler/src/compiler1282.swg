#[Swag.Mixin]
func toto(x: code)
{
    if #mixin x:
        @assert(true)
    else:
        @assert(false)
}

//#[Swag.PrintBc]
#test
{
    let a = 1
    let b = 2
    toto(#code a < b)
}