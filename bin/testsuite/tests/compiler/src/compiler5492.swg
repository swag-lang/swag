/*#global testerror "Err0354"

//#[Swag.Macro]
func toto(x: string)
    where<titi>
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
}*/