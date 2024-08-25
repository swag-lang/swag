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
    var cpt = 0'u64
    toto()
    {
        cpt += #index
    }

    @assert(cpt == 45)
}