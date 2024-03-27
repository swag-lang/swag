#global testerror "Err0039"

struct V {}

//#[Swag.Macro]
func toto(x: V)
    #validif
    {
        var y = x
        @print(y)
        return false
    }
{
}

#test
{
    toto({})
}