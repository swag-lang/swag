#[Swag.ConstExpr]
func toto()->bool
{
    var x = 1
    return cast(bool) (x & 1)
}

#assert toto()