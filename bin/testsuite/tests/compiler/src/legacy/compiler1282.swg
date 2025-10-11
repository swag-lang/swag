#[Swag.Mixin]
func toto(x: #code void)
{
    if #inject(x) do
        @assert(true)
    else do
        @assert(false)
}

//#[Swag.PrintBc]
#test
{
    let a = 1
    let b = 2
    toto(#code a < b)
}
