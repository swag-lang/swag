	/* salut la foule
		/* */
	*/

//#[Swag.ConstExpr]
func orFlags()->Swag.TypeInfoFlags
{
    var res: Swag.TypeInfoFlags = Zero
    return res
}

#test
{
    if orFlags() & .Zero
    {
    }
}