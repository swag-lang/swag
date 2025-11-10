#global testerror "Err0378"

struct A {}

#test
{
    var x: A
    var y  = x[0]
}
