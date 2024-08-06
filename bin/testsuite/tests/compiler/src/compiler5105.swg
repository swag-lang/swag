#global testerror "Err0248"

struct A {}

#test
{
    var x: A
    var y = x[0]
}