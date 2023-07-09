struct V {}

impl V
{
    #[Swag.Inline]
    mtd opCount()->u64
    {
        return 5
    }

    mtd opSlice(l, u: u64)->string
    {
        @assert(l == 0)
        @assert(u == 5)
        return "toto"
    }
}

#test
{
    var v: V
    var y = v[0..<6]
    @assert(y == "toto")
}
