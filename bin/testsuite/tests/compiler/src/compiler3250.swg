#global testerror "Err0702"
func toto()->{x: s32}
{
    return {1}
}

#test
{
    var a, b: s32
    (a, b) = toto()
}