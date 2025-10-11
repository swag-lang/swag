//#[Swag.PrintBc]
#[Swag.Sanity(true), Swag.Warning("Wrn0003", Swag.WarningLevel.Disable)]
func toto()
{
    var a: [4] s32

    var x = -1
    while x >= 0
    {
        @assert(a[x] == 0)
    }
}
