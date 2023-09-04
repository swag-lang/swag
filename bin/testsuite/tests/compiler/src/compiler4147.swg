#global testerror "Err0116"
#global testerror "Err0117"

struct A
{
}

struct B {}

impl A
{
    func toto(ptr1: *A)
    {
    }

    func toto(ptr1: *B)
    {
    }

    mtd titi(using ptr2: *B)
    {
        toto()
    }
}