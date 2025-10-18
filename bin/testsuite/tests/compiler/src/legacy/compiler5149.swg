#global testerror "Err0098"

struct A {}

impl A
{
    mtd titi() {}
}

#test
{
    var a: A
    a.toto()
}
