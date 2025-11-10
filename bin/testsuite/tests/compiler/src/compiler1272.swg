#[Swag.ConstExpr]
func toto()->bool
{
    let x = 1
    return cast(bool) (x & 1)
}

#assert(toto())
