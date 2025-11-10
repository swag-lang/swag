enum RGB { R, G, B }

struct A {}
impl A
{
    mtd toto(rgb: RGB) {}
}

func titi(x: bool, rgb: RGB) {}

#test
{
    var x: A
    x.toto(.R)

    var x1: bool
    x1.titi(.G)
}
