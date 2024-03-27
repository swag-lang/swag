#global testerror "Err0258"

struct A {}

#test
{
    var x: A
    var y = x[0]
}