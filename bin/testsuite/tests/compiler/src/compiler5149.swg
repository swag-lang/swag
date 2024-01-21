#global testerror "Err0726"

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