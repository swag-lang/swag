//#[Swag.PrintBc]
#[Swag.Warning("Wrn0003", Swag.WarningLevel.Disable)]
#test
{
    var c  = 666
    let d: [..] s32 = [1, c, 2]

    @assert(d[0] == 1)
    @assert(d[2] == 2)
    @assert(d[1] == 666)
}
