#global testerror "Err0725"

#test
{
    struct A {}
    struct B { v: A; }
    var x: B
    x.x = 0
}