#global testerror "Err0056"

#[Swag.Macro]
func toto(x: string)->string
{
    #ast
    {
        let y = x
        @print(y)
        return "return \"5\""//"return \"" ++ x ++ "\""
    }
}

//#[Swag.Warning("Wrn0002|Wrn0006", Swag.WarningLevel.Disable)]
#test
{
    var y = "5"
    let x = toto(y)
    @assert(x == "5")
}