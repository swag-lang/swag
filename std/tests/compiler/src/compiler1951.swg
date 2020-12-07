struct(A) Foozle
{
}

var GG = 0

impl Foozle
{
    func add(self, x: A)
    {
        GG = 1
    }

    func(G) add1(self, x: G)
    {
        var t: A = cast(A) x
        self.add(t)
    }
}

#test
{
    var fooz: Foozle's32
    fooz.add1(0'f32)
    @assert(GG == 1)
}