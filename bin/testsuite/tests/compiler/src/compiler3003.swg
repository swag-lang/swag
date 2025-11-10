struct R
{
    x:   s32
}

#[Swag.Macro]
func vis(stmt: #code void)
{
    #inject(stmt)
}

func toto()->R
{
    var result: retval

    vis()
    {
        result.x += 1
    }

    return result
}

#test
{
    var x = toto()
}
