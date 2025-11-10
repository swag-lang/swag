//#global testerror "Err0371"
#global testerror "Err0320"

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
