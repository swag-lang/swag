#global testerror "Err0175"

struct A {}

#test
{
    var x: A
    var y  = x[0]
}
