#global testerror "Err0589"
#test
{
    typealias X = *u16
    var v: X
    var str = @mkstring(v, 1)
}