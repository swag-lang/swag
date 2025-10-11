var G = 0

#[Swag.Macro]
func toto(stmt: #code void)
{
    #macro
    {
        var x = 0's64
        defer G = 1
        #inject(#up stmt)
    }
}

#test
{
    toto()
    {
    }

    @assert(G == 1)
}
