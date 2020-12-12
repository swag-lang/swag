#[swag.macro]
func toto(x: code)
{
    loop 10
    {
        #macro
        {
            #mixin `x
        }
    }
}

#test
{
    cpt := 0'uint
    toto()
    {
        cpt += @index
    }

    @assert(cpt == 45)
}