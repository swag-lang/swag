public struct A74
{
    a: s32
}

public impl A74
{
    struct Toto74
    {
        x: s32
    }

    mtd titi74(a: Toto74 = {})
    {
        //@print("a")
    }
}

#test
{
    var a: A74
    a.titi74()
}