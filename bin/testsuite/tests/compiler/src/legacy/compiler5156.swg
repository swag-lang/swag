#global testerror "Wrn0003"
#global #[Swag.Warning("Wrn0003", Swag.WarningLevel.Error)]

func toto()->{x, y: s32}
{
    return {1, 2}
}

#test
{
    var (a, b) = toto()
    @print(a)
}