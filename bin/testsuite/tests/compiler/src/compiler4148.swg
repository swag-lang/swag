#global testerror "Err0480"
struct A
{
}

struct B {}

impl B
{
    func toto()
    {
    }
}

impl A
{
    mtd titi(using ptr: *B)
    {
        ptr.toto()
    }
}