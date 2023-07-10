#global testerror

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

#test
{
    let y = "5"
    let x = toto(y)
    @assert(x == "5")
}