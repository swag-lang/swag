//#global testerror "Err0483"
#global testerror "Err0452"

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