#global testerror "Err0041"

struct V {}

//#[Swag.Macro]
func toto(x: V)
    where
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