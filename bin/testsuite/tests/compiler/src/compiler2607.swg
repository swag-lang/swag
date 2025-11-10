var G = 0

#[Swag.Macro]
func titi(x: s32, stmt: #code void)
{
    func toto(x: s32)
    {
        func tata(x: s32)
        {
            #inject(stmt)
        }

        tata(x)
    }

    toto(x)
}

#test
{
    let v = 6
    titi(v)
    {
        @assert(x == 6)
        G = 1
    }

    @assert(G == 1)
}
