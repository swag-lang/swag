#global testerror "Err0702"
func toto()->{x: s32, y: s32}
{
    return {1, 2}
}

#test
{
    var (a, b, c) = toto()
}