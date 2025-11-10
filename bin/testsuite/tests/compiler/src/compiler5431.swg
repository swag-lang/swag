namespace A
{
    alias T = s32
}

#[Swag.Using(A)]
func toto()->{ a: T }
{
    return {0}
}
