#global testerror "Err0156"

struct A {}

#test
{
    var x: A
    var y = x[0]
}