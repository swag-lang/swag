#global testerror

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
    x := toto("5")
}