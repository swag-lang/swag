#[Swag.ConstExpr]
func orFlags()->Swag.TypeInfoFlags
{
    var res: Swag.TypeInfoFlags = Zero
    return res
}

#if orFlags() & .Zero
{
}
