#global testerror "Err0368"

#[Swag.Macro]
func toto(#[Swag.ConstExpr] x: string)->string
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