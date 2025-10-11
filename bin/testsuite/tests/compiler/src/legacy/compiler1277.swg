#[Swag.Mixin]
func toto(x: #code void)
{
    #inject(x)
    #inject(x)
    #inject(x)
}

//#[Swag.PrintBc]
#test
{
    var b = 0
    toto(#code { b += 1; })
    @assert(b == 3)
}
