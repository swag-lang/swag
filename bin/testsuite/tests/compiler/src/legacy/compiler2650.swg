#[Swag.Macro]
func toto(x: s64 = 666, stmt: #code void)
{
    #inject(stmt)
}

#test
{
    toto()
    {
        @assert(x == 666)
    }
}
