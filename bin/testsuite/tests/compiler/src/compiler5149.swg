#global testerror "Err1107"

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