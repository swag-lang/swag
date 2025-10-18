//#global testerror "Err0609"
#global testerror "Err0280"

struct A {}
interface I
{
    mtd x();
}

impl I for A
{
    mtd opDrop() {}
    mtd impl x() {}
}

#test
{
    var x: A
}
