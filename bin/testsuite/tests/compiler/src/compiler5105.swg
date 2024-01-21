#global testerror "Err0259"

struct A {}

#test
{
    var x: A
    var y = x[0]
}