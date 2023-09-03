#global testerror "Err0206"

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

//#[Swag.Warn("wrn0003|wrn0004", Swag.WarnLevel.Disable)]
#test
{
    var y = "5"
    let x = toto(y)
    @assert(x == "5")
}