//#global skip

struct A
{
    x: closure()
}

impl A
{
    mtd opAffect(p: closure())
    {
        x = p
    }
}

//#[Swag.PrintBc]
#test
{
    var x: A
    x = func() {}
}