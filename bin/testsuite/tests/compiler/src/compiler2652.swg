#[Swag.Macro]
func toto(x, y: s64 = 666, stmt: #code void)
{
    @assert(x == 444)
    @assert(y == 666)
    #inject(stmt)
}

#test
{
    toto(x: 444)
    {
        @assert(x == 444)
        @assert(y == 666)
    }

    /*toto(y: 444)
    {
        @assert(x == 666)
        @assert(y == 444)
    }*/
}
