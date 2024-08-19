#global testerror "Err0559"

#[Swag.Macro]
func toto(x: string)->string
{
    #ast
    {
        return 0//"return " ++ x
    }
}

#test
{
    var x = toto("5")
}