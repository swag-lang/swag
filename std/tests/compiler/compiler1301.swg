#[swag.macro]
func toto(x: code)
{
    loop 10
    {
        #inline
        {
            #mixin x
        }
    }
}

#test
{
    cpt := 0
    toto()
    {
        cpt += 1
        break
    }
    
    @assert(cpt == 1)
}