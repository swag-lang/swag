#global testerror "Err0723"

#test
{
    struct A {}
    struct B { v: A; }
    var x: B
    x.x = 0
}