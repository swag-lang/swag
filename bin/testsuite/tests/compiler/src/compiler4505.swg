var v: func(x: s32)

#[Swag.Mixin]
func toto()
{
    {
        v = func(btn) {}
    }
}

#test
{
    toto()
}
