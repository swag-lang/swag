#[Swag.Macro]
func toto(stmt: code)
{
    scope Up
    {
        loop 10
        {
            #mixin stmt { break = @assert(false); }
        }
    }
}

#[Swag.Macro]
func titi(stmt: code)
{
    loop 5
    {
        #mixin stmt
    }
}

#test
{
    cpt := 0
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