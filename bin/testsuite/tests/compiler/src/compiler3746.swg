interface I
{
    func toto3746(self);
}

struct(T) A {}

impl I for A
{
    mtd(N) titi3746()
    {
        let v = #typeof(N)
        @assert(v.name == "s32")
    }

    mtd impl toto3746()
    {
        self.titi3746's32()
    }
}

#test
{
    var x: A's32
    x.I.toto3746()
}