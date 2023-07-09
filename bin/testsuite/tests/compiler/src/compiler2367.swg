#[Swag.Macro]
func toto(stmt: code)
{
    #scope up
    loop 10
    {
        #macro
        {
            #mixin #up stmt { break = break up; }
        }
    }
}

#test
{
    var cpt = 0
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