public struct A75
{
    a: s32
}

public impl A75
{
    struct Toto75
    {
        x: s32
    }

    mtd titi75(a: Toto75 = Toto75{})
    {
        //@print("a")
    }
}

#test
{
    var a: A75
    a.titi75()
}