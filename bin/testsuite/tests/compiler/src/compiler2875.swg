#[Swag.Macro]
func toto(stmt: code)
{
    let #alias0: s64 = 555
    let #alias1: s64 = 666
    #mixin stmt
}

#test
{
    toto(|x, y| )
    {
        @assert(x == 555)
        @assert(y == 666)
    }
}
