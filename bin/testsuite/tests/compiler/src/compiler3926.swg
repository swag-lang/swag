//#global testerror "Err0427"
#global testerror "Err0397"

struct A {}
interface I
{
    mtd x();
}

impl I for A
{
    mtd opDrop(){}
    mtd impl x(){}
}

#test
{
    var x: A
}