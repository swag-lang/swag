#global testerror "Err0721"
func toto()->{x: s32, y: s32}
{
    return {1, 1}
}

#test
{
    var (a) = toto()
}