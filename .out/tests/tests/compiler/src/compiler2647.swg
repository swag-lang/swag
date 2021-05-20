#[Swag.macro]
func(T) toto(v: T, stmt: code)
{
    func titi(val: T)
    {
        #mixin stmt
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