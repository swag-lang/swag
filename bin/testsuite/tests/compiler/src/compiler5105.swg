#global testerror "Err0198"

struct A {}

#test
{
    var x: A
    var y = x[0]
}