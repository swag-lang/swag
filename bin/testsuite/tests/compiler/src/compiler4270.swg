#global testerror

struct V {}

//#[Swag.Macro]
func toto(x: V)
    #validif
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