#global testerror "Err0197"

struct A {}

#test
{
    var x: A
    var y = x[0]
}