#[swag.macro]
func toto(x: code)
{
    loop 10
    {
        #inline
        {
            #mixin `x
        }
    }
}

#test
{
    cpt := 0'u32
    toto()
    {
        cpt += index
    }
    
    @assert(cpt == 45)
}