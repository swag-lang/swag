#[Swag.Macro]
func toto(stmt: code)
{
    #scope Up
    {
        for 10
        {
            #mixin stmt where { break = @assert(false); }
        }
    }
}

#[Swag.Macro]
func titi(stmt: code)
{
    for 5
    {
        #mixin stmt
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