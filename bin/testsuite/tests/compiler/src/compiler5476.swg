using Swag

struct(T) A {}

impl A
{
    //#[Swag.Inline]
    mtd toto()->A => {}
}

func titi(x: &&A) {}

//#[Swag.PrintBc]
#test
{
    var a: A's32
    titi(a.toto())
}
