#global testerror "Err0559"
#test
{
    alias X = *u16
    var v:  X
    var str = @mkstring(v, 1)
}
