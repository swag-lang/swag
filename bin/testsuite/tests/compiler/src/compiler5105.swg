#global testerror "Err0168"

struct A {}

#test
{
    var x: A
    var y = x[0]
}