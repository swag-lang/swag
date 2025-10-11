func toto(a: *void) {}

#test
{
    struct A {}
    var a: A
    toto(cast() a)
}
