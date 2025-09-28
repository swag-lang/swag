#global #[Swag.Warning("Wrn0002", Swag.WarningLevel.Error)]
#global testerror "Wrn0002"

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
