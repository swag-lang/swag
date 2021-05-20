#[Swag.constexpr]
func toto()->bool
{
    x := 1
    return cast(bool) (x & 1)
}

#assert toto()