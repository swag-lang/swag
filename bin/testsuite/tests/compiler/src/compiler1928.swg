#global testerror "Err0251"

typealias A = [2] s32
var x: A

#test
{
    namealias p = &x[0]
}