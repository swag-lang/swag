#[Swag.Macro]
func toto(x: string)->string
{
    #ast
    {
        return "return \"" ++ x ++ "\""
    }
}

#test
{
    let x = toto("5")
    @assert(x == "5")
}
