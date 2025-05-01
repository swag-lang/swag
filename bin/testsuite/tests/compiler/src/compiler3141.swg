#[Swag.Macro]
func toto(stmt: #block void)
{
    #scope Up
    {
        for 10
        {
            #inject stmt => { break = @assert(false); }
        }
    }
}

#[Swag.Macro]
func titi(stmt: #block void)
{
    for 5
    {
        #inject stmt
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