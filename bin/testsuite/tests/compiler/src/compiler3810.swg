//#global testerror
struct A
{
    x: s32
}

impl A
{
    mtd toto()
    {
    }
}

func aa()
{
    var a: A
    with a
    {
        .toto()
        .x = 0
    }
}
