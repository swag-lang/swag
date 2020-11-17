struct Pt
{
    x: s64 = 1
}

func toto(x: Pt)
{
    //@print(x.x)
}

#test
{
    toto(@{"true"})
}