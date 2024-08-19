//#global testerror "Err0435"
#global testerror "Err0406"

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