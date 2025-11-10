#[Swag.Mixin]
func toto(x: #code void)
{
    #ast
    {
        return "const " ++ #stringof(x) ++ " = \"" ++ #stringof(x) ++ "\""
    }
}

#test
{
    toto(#code Coul)
    toto(#code Coul1)
    @assert(Coul == "Coul")
    @assert(Coul1 == "Coul1")
}
