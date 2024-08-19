#global testerror "Err0739"
func toto()->{x: s32}
{
    return {1}
}

#test
{
    var (a, b) = toto()
}