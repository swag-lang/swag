#global testerror "Err0638"

typealias A = [2] s32
var x: A

#test
{
    namealias p = &x[0]
}