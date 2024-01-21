#global testerror "Err0031"

//#[Swag.Macro]
func toto(x: string)
    #validif
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