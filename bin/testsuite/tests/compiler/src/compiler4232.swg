#global testerror "Err0550"

#[Swag.Macro]
func toto(x: string)->string
{
    #ast
    {
        return "return " ++ x
    }
}

#test
{
    var x = toto("5")
}