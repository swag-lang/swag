interface I
{
    func toto(me);
}

struct(T) A {}

impl I for A
{
    mtd(N) titi()
    {
        let v = #typeof(N)
        @assert(v.name == "s32")
    }

    mtd impl toto()
    {
        titi's32(me)
    }
}

#test
{
    var x: A's32
    x.I.toto()
}
