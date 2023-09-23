#global testerror "Err0227"

struct A {}

#test
{
    var x: A
    var y = x[0]
}