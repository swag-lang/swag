//#global testerror "Err0529"
#global testerror "Err0499"

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