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
    var cpt = 0'u64
    toto()
    {
        cpt += @index
    }

    @assert(cpt == 45)
}
