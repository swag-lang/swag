#[Swag.Macro]
func toto(x: code)
{
    loop 10
    {
        #inline
        {
            value := 100
            #mixin `x
        }
    }
}

#test
{
    cpt := 0'u32
    toto()
    {
        @assert(value == 100)
    }
}