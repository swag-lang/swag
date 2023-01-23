struct V {}

impl V
{
    #[Swag.Inline]
    mtd opCount()->uint
    {
        return 5
    }

    mtd opSlice(l, u: uint)->string
    {
        @assert(l == 0)
        @assert(u == 5)
        return "toto"
    }
}

#test
{
    var v: V
    y := v[0..]
    @assert(y == "toto")
}
