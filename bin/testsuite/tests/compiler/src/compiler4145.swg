struct A
{
}

struct B {}

impl A
{
    /*func toto(ptr1: *A)
    {
    }*/

    func toto(ptr1: *B)
    {
    }

    mtd titi(using ptr2: *B)
    {
        toto()
    }
}