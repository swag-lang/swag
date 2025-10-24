#global #[Swag.Warning("Wrn0002", Swag.WarningLevel.Error)]
#global testerror "Wrn0002"

#[Swag.Deprecated("use something else this instead")]
enum AA
{
}

enum BB
{
    #[Swag.Deprecated("use this instead")]
    A
}

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
    var x: AA = undefined
    var y  = BB.A

    titi()
}
