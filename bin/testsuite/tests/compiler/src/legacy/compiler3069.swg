struct AA {}

impl AA
{
    mtd opCount()->u64
    {
        return 1
    }

    mtd opSlice(a, b: u64)->const [..] s32
    {
        return [1]
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
    const X = toto()
    var v   = X[0]
}
