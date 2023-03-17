#[Swag.Macro]
func toto()
{
    x := 0
    #macro
    {
        x := 0
    }
}

#test
{
    toto()
}