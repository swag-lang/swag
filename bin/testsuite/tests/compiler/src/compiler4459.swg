#[Swag.Macro]
func toto()
{
    x42 := 0
    #macro(x42)
    {
        x42 = 2
    }
    @assert(x42 == 2)
}

#test
{
    toto()
}