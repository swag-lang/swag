#[Swag.Macro]
func toto(stmt: #code void)
{
    #scope(up)
    for 10
    {
        #macro
        {
            #inject(#up stmt, break = break to up)
        }
    }
}

#test
{
    var cpt = 0
    toto()
    {
        for 4
        {
            cpt += 1
            break
        }
    }

    @assert(cpt == 10)
}
