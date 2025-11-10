struct Argb
{
    a:   u64
}

impl Argb
{
    #[Swag.Inline]
    func extract(me)->{ a, b: s64 }
    {
        return {666, 555}
    }
}

//#[Swag.PrintBc]
#test
{
    var color: Argb
    let (a, b) = color.extract()
    @assert(a == 666)
    @assert(b == 555)
}
