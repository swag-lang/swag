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
    //var x = &AA.toto
    @assert(#nameof(#typeof(AA.toto)) == "func(*AA)->s32")
    //var v: AA
    //@assert(v.x() == 10)
}