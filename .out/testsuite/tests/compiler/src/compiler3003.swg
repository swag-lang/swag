struct R
{
    x: s32
}

#[Swag.macro]
func vis(stmt: code)
{
    #mixin stmt
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
    x := toto()
}