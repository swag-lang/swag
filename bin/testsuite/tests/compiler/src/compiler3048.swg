/* salut la foule
		/* */
	*/

//#[Swag.ConstExpr]
func orFlags()->Swag.TypeInfoFlags
{
    let res: Swag.TypeInfoFlags = Zero
    return res
}

#test
{
    if orFlags() & .Zero
    {
    }
}
