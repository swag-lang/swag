#global testerror "Err0027"

//#[Swag.Macro]
func toto(x: string)
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
    var x = "val"
    toto(x)
}