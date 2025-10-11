struct(A) Foozle {}

var GG = 0

impl Foozle
{
    func add(me, x: A)
    {
        GG = 1
    }

    func(G) add1(me, x: G)
    {
        let t: A = cast(A) x
        me.add(t)
    }
}

#test
{
    var fooz: Foozle's32
    fooz.add1(0'f32)
    @assert(GG == 1)
}
