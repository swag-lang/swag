#global testerror "Err0170"

struct A {}

#test
{
    var x: A
    var y = x[0]
}