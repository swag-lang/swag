#global testerror "Err0160"

typealias A = [2] s32
var x: A

#test
{
    namealias p = &x[0]
}