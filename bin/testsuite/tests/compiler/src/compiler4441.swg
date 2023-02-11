#global skip
using Swag

struct(T) A {}
impl A
{
    mtd toto()->A => {}
}

func titi(x: moveref A) {}

//#[Swag.PrintBc]
#test
{
    var a: A's32
    titi(moveref &a.toto())
}
