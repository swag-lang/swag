#[Swag.macro]
func toto(stmt: code)
{
    label up
    loop 10
    {
        #macro
        {
            #mixin `stmt { break = break up; }
        }
    }
}

#test
{
    cpt := 0
    toto()
    {
        loop 4
        {
            cpt += 1
            break
        }
    }

    @assert(cpt == 10)
}