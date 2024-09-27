//#global skip

struct A
{
    x: func||()
}

impl A
{
    mtd opAffect(p: nn func||())
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