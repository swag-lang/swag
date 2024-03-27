//#[Swag.PrintBc]
#[Swag.Safety("sanity", true), Swag.Warn("Wrn0003", Swag.WarnLevel.Disable)]
func toto()
{
    var a: [4] s32

    var x = -1
    while x >= 0
    {
        @assert(a[x] == 0)
    }
}
