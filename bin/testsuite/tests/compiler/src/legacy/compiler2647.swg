#[Swag.Macro]
func(T) toto(v: T, stmt: #code void)
{
    func titi(val: T)
    {
        #inject(stmt)
    }

    titi(v)
}

#test
{
    toto(666's32)
    {
        @assert(val == 666)
    }

    toto("666")
    {
        @assert(val == "666")
    }
}
