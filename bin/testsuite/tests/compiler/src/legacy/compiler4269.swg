#global testerror "Err0477"

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
