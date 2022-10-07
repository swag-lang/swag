func toto(a: *void)
{
}

#test
{
    struct A {}
    var a: A
    toto(acast a)
}