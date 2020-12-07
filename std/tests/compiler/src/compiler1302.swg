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
    cpt := 0'u32
    toto()
    {
        cpt += @index
    }
    
    @assert(cpt == 45)
}