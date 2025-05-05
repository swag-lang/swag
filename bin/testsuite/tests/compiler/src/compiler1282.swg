#[Swag.Mixin]
func toto(x: #code void)
{
    if #inject(x):
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