#global testerror "Err0155"

struct A {}

#test
{
    var x: A
    var y = x[0]
}