//#global testerror "Err0536"
#global testerror "Err0509"

struct A {}
interface I
{
    mtd x()
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