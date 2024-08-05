#global testerror "Err0714"

#test
{
    struct A {}
    struct B { v: A; }
    var x: B
    x.x = 0
}