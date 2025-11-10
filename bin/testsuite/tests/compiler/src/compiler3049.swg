#[Swag.ConstExpr]
func orFlags()->Swag.TypeInfoFlags
{
    let res: Swag.TypeInfoFlags = Zero
    return res
}

#if orFlags() & .Zero
{
}
