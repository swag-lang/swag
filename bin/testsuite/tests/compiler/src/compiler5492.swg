#global testerror "Err0169"

//#[Swag.Macro]
func toto(x: string)
    where:titi
    {
        let y = x
        @print(y)
        return false
    }
{
}

#test
{
    var x = "val"
    toto(x)
}