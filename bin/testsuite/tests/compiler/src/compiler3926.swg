//#global testerror "Err0535"
#global testerror "Err0508"

struct A
{
}
interface I
{
    mtd x();
}

impl I for A
{
    mtd opDrop()
    {
    }
    mtd impl x()
    {
    }
}

#test
{
    var x: A
}
