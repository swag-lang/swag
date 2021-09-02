struct Pt
{
    x: s64 = 1
}

func toto(x: Pt)
{
    @assert(x.x == 555)
}

func offset()->s64 { return 555; }

#test
{
    toto(Pt{offset()})
}