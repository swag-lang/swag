struct(T) Random {}

impl Random
{
    func nextS64(using me)->u64 => nextU64(me)
    func nextU64(using me)->u64 => 1
}

#test
{
    var rng: Random'bool
    var r    = rng.nextS64()
}
