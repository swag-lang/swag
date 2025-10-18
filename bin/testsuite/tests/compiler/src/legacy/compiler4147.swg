//#global testerror "Err0004"
//#global testerror "Err0349"

struct A {}

struct B {}

impl A
{
    #[Swag.Overload]
    func toto(ptr1: *A) {}

    #[Swag.Overload]
    func toto(ptr1: *B) {}

    mtd titi(ptr2: *B)
    {
        me.toto()
    }
}
