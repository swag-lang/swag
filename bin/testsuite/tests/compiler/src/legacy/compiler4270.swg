#global testerror "Err0477"

struct V {}

//#[Swag.Macro]
func toto(x: V)
    where
    {
        let y = x
        @print(y)
        return false
    }
{
}

#test
{
    toto({})
}
