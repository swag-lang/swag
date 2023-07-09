#global testerror

//#[Swag.Macro]
func toto(x: string)
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
    var x = "val"
    toto(x)
}