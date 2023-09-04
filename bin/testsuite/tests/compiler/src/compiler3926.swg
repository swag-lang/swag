#global testerror "Err0280"
#global testerror "Err0494"

struct A {}
interface I
{
    mtd x()
}

impl I for A
{
    mtd opDrop(){}
    mtd x(){}
}

#test
{
    var x: A
}