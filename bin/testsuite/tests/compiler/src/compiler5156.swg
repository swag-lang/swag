#global testerror "Wrn0009"
#global #[Swag.Warn("Wrn0009", Swag.WarnLevel.Error)]

func toto()->{x, y: s32}
{
    return {1, 2}
}

#test
{
    var (a, b) = toto()
    @print(a)
}