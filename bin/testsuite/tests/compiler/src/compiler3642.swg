#global testerror "Err0040"
struct C
{
}

impl C
{
    mtd opAffect(x: s32)
    {
    }
}

func toto(x: C)
{
}

#test
{
    const c: C = 4
    toto(c)
}
