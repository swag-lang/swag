#[Swag.Macro]
func toto(x: s64 = 666, stmt: #block void)
{
    #inject stmt
}

#test
{
    toto()
    {
        @assert(x == 666)
    }
}
