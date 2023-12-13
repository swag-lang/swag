#global testerror "Err0481"
#test
{
    var x = 0
    var v = cast(&s32) &x
    var z = dref v
}