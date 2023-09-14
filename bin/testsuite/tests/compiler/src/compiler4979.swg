#global #[Swag.Warn("wrn0003", Swag.WarnLevel.Error)]
#global testerror "Wrn0003"

#[Swag.Deprecated("use this instead")]
func titi()
{
    var a = 0
    a += 1
    a += 1
}

//#[Swag.PrintBc]
#test
{
    titi()
}
