#global testerror "Err0254"

struct A {}

#test
{
    var x: A
    var y = x[0]
}