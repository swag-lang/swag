#global testerror "Err0745"
func toto()->{x: s32}
{
    return {1}
}

#test
{
    var a, b: s32
    (a, b) = toto()
}