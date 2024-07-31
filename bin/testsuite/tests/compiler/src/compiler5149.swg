#global testerror "Err0731"

struct A {}

impl A
{
    mtd titi(){}
}

#test
{
    var a: A
    a.toto()
}