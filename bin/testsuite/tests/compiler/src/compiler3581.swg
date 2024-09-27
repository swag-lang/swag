//#global skip

struct A
{
    x: func||()
}

impl A
{
    mtd opAffect(p: func||())
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