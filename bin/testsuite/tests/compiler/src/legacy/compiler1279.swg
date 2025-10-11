#[Swag.Mixin]
func toto(x: #code void)
{
    for 10 do
        #inject(x)
}

//#[Swag.PrintBc]
#test
{
    var b = 0
    toto(#code { b += 1; })
    @assert(b == 10)
}
