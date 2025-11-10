#[Swag.Macro]
func toto(x: #code void)
{
    for 10
    {
        #macro
        {
            #inject(#up x)
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
