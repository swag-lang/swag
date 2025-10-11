struct Duration
{
    val: f32
}

impl Duration
{
    //#[Swag.Implicit]
    mtd(op: string) opAffectLiteral(v: f32)
    {
        @assert(v == 5.0 or v == 6.0)
        #assert(op == "s" or op == "ms")
    }
}

func toto(x: Duration) {}

#test
{
    toto(cast(Duration) 5.0's)
    toto(cast(Duration) 6.0'ms)
}
