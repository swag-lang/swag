#global testerror

//#[Swag.Macro]
func toto(x: string)
    #selectif
    {
        y := x
        @print(y)
        return false
    }
{
}

#test
{
    x := "val"
    toto(x)
}