#global testerror "Err0157"

struct A {}

#test
{
    var x: A
    var y = x[0]
}