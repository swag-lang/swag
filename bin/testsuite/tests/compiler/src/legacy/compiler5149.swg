#global testerror "Err0096"

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
