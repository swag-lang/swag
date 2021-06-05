#[Swag.macro]
func toto(x: s64 = 666, stmt: code)
{
    #mixin stmt
}

#test
{
    toto()
    {
        @assert(x == 666)
    }
}
