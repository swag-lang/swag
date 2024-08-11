#global testerror "Err0718"
func toto()->{x: s32}
{
    return {1}
}

#test
{
    var a, b: s32
    (a, b) = toto()
}