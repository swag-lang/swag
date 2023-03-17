#[Swag.Macro]
func toto(x: code)
{
    loop 10
    {
        #macro
        {
            value := 100
            #mixin #up x
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