#global #[Swag.Warn("wrn0003", Swag.WarnLevel.Error)]
#global testerror "Wrn0003"

enum BB
{
    #[Swag.Deprecated("use this instead")]
    A
}

//#[Swag.PrintBc]
#test
{
    var y = BB.A
}
