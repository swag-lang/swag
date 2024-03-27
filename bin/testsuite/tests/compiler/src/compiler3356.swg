struct A
{
    x: s32
}

impl A
{
    mtd toto(x: s32)
    {
        self.x = #self.x
    }
}

#test
{
    var v: A
    v.toto(10)
    @assert(v.x == 10)
}
