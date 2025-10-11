#[Swag.Macro]
func toto(stmt: #code void)
{
    func titi()
    {
        let #alias0: u32 = 666
        #inject(stmt)
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
