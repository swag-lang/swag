struct AA
{
}

impl AA
{
    method opCount()->uint
    {
        return 4
    }

    method opSlice(a, b: uint)->const [..] string
    {
        return @["1", "2", "3" , "4"]
    }
}

#[Swag.ConstExpr]
func toto()->AA
{
    return @{}
}

//#[Swag.PrintBc]
#test
{
    const X = #run toto()
    v := X[0]
    v1 := v
    @assert(v1 == "1")
    @assert(X[1] == "2")
    @assert(X[2] == "3")
    @assert(X[3] == "4")
}
