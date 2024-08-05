#global testerror "Err0720"

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