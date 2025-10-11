#[Swag.Macro]
func toto(stmt: #code void)
{
    #scope(Up)
    {
        for 10
        {
            #inject(stmt, break = @assert(false))
        }
    }
}

#[Swag.Macro]
func titi(stmt: #code void)
{
    #scope(Up)
    for 5
    {
        for 5
        {
            #inject(stmt, break = break to Up)
        }
    }
}

#test
{
    var cpt = 0
    toto()
    {
        titi()
        {
            cpt += 1
            //@print("x")
            break
        }
    }

    @assert(cpt == 10)
}
