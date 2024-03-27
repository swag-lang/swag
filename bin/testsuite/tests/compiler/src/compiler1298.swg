#[Swag.Macro]
func toto()
{
    var x = 0
    #macro
    {
        var x = 0
    }
}

#test
{
    toto()
}
