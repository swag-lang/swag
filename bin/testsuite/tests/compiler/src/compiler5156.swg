#global testerror "Wrn0003"
#global #[Swag.Warn("Wrn0003", Swag.WarnLevel.Error)]

func toto()->{x, y: s32}
{
    return {1, 2}
}

#test
{
    var (a, b) = toto()
    @print(a)
}
