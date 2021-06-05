struct AA
{
    xx: s32 = 10
}

impl AA
{
    func toto(using self) => xx
}

#test
{
    //x := &AA.toto
    @assert(@stringof(AA.toto) == "(*AA)->s32")
    //var v: AA
    //@assert(v.x() == 10)
}