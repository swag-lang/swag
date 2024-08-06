#global testerror "Err0708"

#test
{
    struct A {}
    struct B { v: A; }
    var x: B
    x.x = 0
}