#global testerror "Err0166"

struct A {}

#test
{
    var x: A
    var y = x[0]
}