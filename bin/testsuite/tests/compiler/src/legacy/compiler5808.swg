#global testerror "Err0215"
interface I5806
{
    mtd a() { b(); }
    mtd b(); // {}
}

struct B {}

impl I5806 for B
{
    /*mtd impl b()
    {
    }    */
}
