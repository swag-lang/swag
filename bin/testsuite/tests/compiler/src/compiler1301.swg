#[Swag.Macro]
func toto(x: code)
{
    for 10
    {
        #macro
        {
            #mixin #up x
        }
    }
}

#test
{
    var cpt = 0
    toto()
    {
        cpt += 1
        break
    }

    @assert(cpt == 1)
}