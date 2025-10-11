//#[Swag.PrintBc]
#[Swag.Warning("Wrn0003", Swag.WarningLevel.Disable)]
#test
{
    var a = 666
    if a == 666
    {
        func toto()
        {
            var a = 1
        }

        toto()
    }

    @assert(a == 666)
}
