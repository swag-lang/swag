#global testerror

struct V {}

//#[Swag.Macro]
func toto(x: V)
    #selectifonce
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