#[Swag.Macro]
func toto(stmt: code)
{
    #scope Up
    {
        loop 10
        {
            #mixin stmt where { break = @assert(false); }
        }
    }
}

#[Swag.Macro]
func titi(stmt: code)
{
    #scope Up
    loop 5
    {
        loop 5
        {
            #mixin stmt where { break = break Up; }
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