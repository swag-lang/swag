struct AA {}

impl AA
{
    mtd opCount()->u64
    {
        return 4
    }

    mtd opSlice(a, b: u64)->const [..] string
    {
        return ["1", "2", "3", "4"]
    }
}

#[Swag.ConstExpr]
func toto()->AA
{
    return {}
}

//#[Swag.PrintBc]
#test
{
    const X = #run toto()
    let v   = X[0]
    let v1  = v
    @assert(v1 == "1")
    @assert(X[1] == "2")
    @assert(X[2] == "3")
    @assert(X[3] == "4")
}
