#global testerror "Err0024"
#global testerror "Err0019"

struct A
{
}

struct B {}

impl A
{
    #[Swag.Overload]
    func toto(ptr1: *A)
    {
    }

    #[Swag.Overload]
    func toto(ptr1: *B)
    {
    }

    mtd titi(using ptr2: *B)
    {
        toto()
    }
}