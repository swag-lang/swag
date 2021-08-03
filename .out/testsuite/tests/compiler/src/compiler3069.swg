struct AA
{
}

impl AA
{
    method opCount()->uint
    {
        return 1
    }

    method opSlice(a, b: uint)->const [..] s32
    {
        return @[1]
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
    const X = toto()
    v := X[0]
}
