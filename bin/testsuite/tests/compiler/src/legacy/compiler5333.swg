#global testerror "Err0282"

struct A
{
    z:   f32
}

impl A
{
    mtd titi() {}
}

func toto(x: A)
{
    x.titi()
}
