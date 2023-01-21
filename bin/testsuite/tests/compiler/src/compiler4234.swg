#global testerror

#[Swag.Macro]
func toto(x: string)->string
{
    #ast
    {
        y := x
        @print(y)
        return "return \"5\""//"return \"" ++ x ++ "\""
    }
}

#test
{
    y := "5"
    x := toto(y)
    @assert(x == "5")
}