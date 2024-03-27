#global testerror "Err0729"
func toto()->{x: s32}
{
    return {1}
}

#test
{
    var (a, b) = toto()
}