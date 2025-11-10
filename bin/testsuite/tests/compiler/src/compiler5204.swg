struct A { x: s32 }

#[Swag.Warning("Wrn0003", Swag.WarningLevel.Disable), Swag.Inline]
func toto()
{
    let err = 0
    var a:  A
    a = A{err}
}

#test
{
    toto()
}
