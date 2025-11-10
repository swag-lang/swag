struct A {}

struct B {}

impl A
{
    /*func toto(ptr1: *A)
    {
    }*/

    func toto(ptr1: *B) {}

    mtd titi(ptr2: *B)
    {
        ptr2.toto()
    }
}
