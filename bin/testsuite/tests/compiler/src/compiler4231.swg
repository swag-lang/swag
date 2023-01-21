#global testerror

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
    x := toto("5")
}