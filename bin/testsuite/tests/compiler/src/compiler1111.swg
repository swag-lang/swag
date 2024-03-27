struct Mwc
{
    val: u32
}

impl Mwc
{
    func next(using self)->u32
    {
        val += 1
        return val
    }
}

struct(T = Mwc) Random
{
    rng: T
}

impl Random
{
    using Swag
    func nextU32(using self)->u32 => rng.next()
}

#test
{
    var rnd: Random
}
