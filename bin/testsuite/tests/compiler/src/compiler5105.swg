#global testerror "Err0195"

struct A {}

#test
{
    var x: A
    var y = x[0]
}