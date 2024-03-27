#global testerror "Err0081"

#[Swag.Macro]
func toto(x: string)->string
{
    #ast
    {
        let y = x
        @print(y)
        return "return \"5\"" //"return \"" ++ x ++ "\""
    }
}

//#[Swag.Warn("Wrn0002|Wrn0006", Swag.WarnLevel.Disable)]
#test
{
    var y = "5"
    let x = toto(y)
    @assert(x == "5")
}
