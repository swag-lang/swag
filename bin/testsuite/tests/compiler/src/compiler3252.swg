#global testerror "Err0723"
func toto()->{x: s32, y: s32}
{
    return {1, 1}
}

#test
{
    var (a) = toto()
}