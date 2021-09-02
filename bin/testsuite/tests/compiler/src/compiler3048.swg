	/* salut la foule
		/* */
	*/

//#[Swag.ConstExpr]
func orFlags()->Swag.TypeInfoFlags
{
    var res: Swag.TypeInfoFlags = None
    return res
}

#test
{
    if orFlags() & .None
    {
    }
}