#[Swag.Macro]
func toto(stmt: #code void)
{
    let #alias0: s64 = 555
    let #alias1: s64 = 666
    #inject(stmt)
}

#test
{
    toto(|x, y|)
    {
        @assert(x == 555)
        @assert(y == 666)
    }
}
