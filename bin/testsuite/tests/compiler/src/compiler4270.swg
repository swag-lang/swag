#global testerror

struct V {}

//#[Swag.Macro]
func toto(x: V)
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
    toto({})
}