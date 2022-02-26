#[Swag.Macro]
func toto(stmt: code)
{
    func titi()
    {
        var @alias0: u32 = 666
        #mixin stmt
    }

    titi()
}

#test
{
    toto(|i|)
    {
        @assert(i == 666)
    }

    toto(|j|)
    {
        @assert(j == 666)
    }
}