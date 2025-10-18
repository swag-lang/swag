/*interface I5806
{
    mtd a() { b(); }
    mtd b();// {}
}

struct A {}

impl I5806 for A
{
    mtd impl b()
    {
    }
}
*/

#global testerror "Err0411"
func titi555();

//func titi555(){}

#test
{
    titi555()
}
