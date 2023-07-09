#global testerror
#global #[Swag.Warn("wrn0003", Swag.WarnLevel.Error)]

#[Swag.Deprecated("use this instead")]
enum AA
{
}

enum BB
{
    #[Swag.Deprecated("use this instead")]
    A
}

#[Swag.Deprecated("use this instead")]
struct CC
{
}

var cc: CC

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
    var y = BB.A

    titi()
}
