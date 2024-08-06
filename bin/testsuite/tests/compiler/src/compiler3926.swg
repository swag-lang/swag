//#global testerror "Err0475"
#global testerror "Err0444"

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