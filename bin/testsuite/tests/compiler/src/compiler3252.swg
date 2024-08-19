#global testerror "Err0735"
func toto()->{x: s32, y: s32}
{
    return {1, 1}
}

#test
{
    var (a) = toto()
}