#global testerror "Err0202"

struct A {}

#test
{
    var x: A
    var y = x[0]
}