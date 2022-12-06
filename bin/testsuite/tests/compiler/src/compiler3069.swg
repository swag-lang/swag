struct AA
{
}

impl AA
{
    mtd opCount()->uint
    {
        return 1
    }

    mtd opSlice(a, b: uint)->const [..] s32
    {
        return [1]
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
