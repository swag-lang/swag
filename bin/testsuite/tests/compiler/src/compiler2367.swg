#[Swag.Macro]
func toto(stmt: code)
{
    #scope up
    for 10
    {
        #macro
        {
            #mixin #up stmt where { break = break in up; }
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