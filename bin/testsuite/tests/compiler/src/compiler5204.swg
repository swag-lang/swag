struct A{x: s32}

#[Swag.Warn("Wrn0003", Swag.WarnLevel.Disable)]
#[Swag.Inline]
func toto()
{
    var err = 0
    var a: A
    a = A{err}
}

#test
{
    toto()
}