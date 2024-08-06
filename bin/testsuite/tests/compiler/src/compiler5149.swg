#global testerror "Err0714"

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