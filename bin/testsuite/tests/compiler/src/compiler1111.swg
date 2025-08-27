struct Mwc
{
    val: u32
}

impl Mwc
{
    func next(me)->u32
    {
        me.val += 1
        return me.val
    }
}

struct(T = Mwc) Random
{
    rng: T
}

impl Random
{
    using Swag
    func nextU32(me)->u32 => rng.next()
    //func nextF32(me)->f32 => rng.next() / cast(f32) u32_max
}

#test
{
    var rnd: Random'Mwc
}
