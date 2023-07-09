#global testerror

#[Swag.Macro]
func toto(x: string)->string
{
    #ast
    {
        var y = x
        @print(y)
        return "return \"5\""//"return \"" ++ x ++ "\""
    }
}

#test
{
    var y = "5"
    var x = toto(y)
    @assert(x == "5")
}